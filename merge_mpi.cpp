#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <random>

using namespace std;

vector<int> merge(const vector<int>& left, const vector<int>& right) {
    vector<int> result;
    result.reserve(left.size() + right.size());
    int l = 0, r = 0;

    while (l < left.size() && r < right.size()) {
        if (left[l] < right[r])
            result.push_back(left[l++]);
        else
            result.push_back(right[r++]);
    }
    while (l < left.size()) result.push_back(left[l++]);
    while (r < right.size()) result.push_back(right[r++]);

    return result;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int procRank, procNum;
    MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);

    int total_size = atol(argv[1]);

    vector<int> data;
    vector<int> counts(procNum), displs(procNum);
    vector<int> local;

    if (procRank == 0) {
        data.resize(total_size);
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dist(1, 1000000000);
        for (int i = 0; i < total_size; ++i) {
            data[i] = dist(gen);
        }

        int chunk_size = (total_size + procNum - 1) / procNum;
        int offset = 0;
        for (int i = 0; i < procNum; ++i) {
            counts[i] = min(offset + chunk_size, total_size) - offset;
            displs[i] = offset;
            offset += counts[i];
        }
    }

    // Send size
    int local_count;
    MPI_Scatter(counts.data(), 1, MPI_INT, &local_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    local.resize(local_count);

    // Send sub-arrays
    MPI_Scatterv(data.data(), counts.data(), displs.data(), MPI_INT,
                 local.data(), local_count, MPI_INT,
                 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    // Sort each sub-array
    sort(local.begin(), local.end());

    // Merge
    int step = 1;
    while (step < procNum) {
        if (procRank % (2 * step) == 0) {
            if (procRank + step < procNum) {
                int recv_size;
                MPI_Status status;

                MPI_Recv(&recv_size, 1, MPI_INT, procRank + step, 0, MPI_COMM_WORLD, &status);
                vector<int> recv_data(recv_size);
                MPI_Recv(recv_data.data(), recv_size, MPI_INT, procRank + step, 0, MPI_COMM_WORLD, &status);

                local = merge(local, recv_data);
            }
        } else {
            int dest = procRank - step;
            int local_size = local.size();

            MPI_Send(&local_size, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(local.data(), local_size, MPI_INT, dest, 0, MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }

    double end_time = MPI_Wtime();

    if (procRank == 0) {
        cout << "Total time: " << (end_time - start_time) << " seconds" << endl;
        cout << "First element: " << local[0] << endl;
        cout << "Last element: " << local.back() << endl;
    }

    MPI_Finalize();
    return 0;
}
