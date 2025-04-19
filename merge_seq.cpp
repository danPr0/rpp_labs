#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <chrono>

using namespace std;

vector<int> merge(const vector<int>& left, const vector<int>& right) {
    vector<int> result;
    result.reserve(left.size() + right.size());
    size_t l = 0, r = 0;

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
    int total_size = atol(argv[2]);
    int num_chunks = atoi(argv[4]);

    vector<int> data;
    data.resize(total_size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 1000000000);
    for (int i = 0; i < total_size; ++i) {
        data[i] = dist(gen);
    }

    auto start = chrono::high_resolution_clock::now();

    vector<vector<int>> chunks;
    int chunk_size = (total_size + num_chunks - 1) / num_chunks;
    for (int i = 0; i < num_chunks; ++i) {
        int start_index = i * chunk_size;
        if (start_index >= total_size) break;

        int end_index = min(start_index + chunk_size, total_size);
        vector<int> chunk(data.begin() + start_index, data.begin() + end_index);
        sort(chunk.begin(), chunk.end());
        chunks.push_back(move(chunk));
    }

    while (chunks.size() > 1) {
        vector<vector<int>> new_chunks;
        for (int i = 0; i < chunks.size(); i += 2) {
            if (i + 1 < chunks.size()) {
                new_chunks.push_back(merge(chunks[i], chunks[i + 1]));
            } else {
                new_chunks.push_back(move(chunks[i]));
            }
        }
        chunks = move(new_chunks);
    }

    vector<int> sorted_data = move(chunks[0]);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;

    cout << "Total time: " << duration.count() / 1000 << " seconds" << endl;
    cout << "First element: " << sorted_data.front() << endl;
    cout << "Last element: " << sorted_data.back() << endl;

    return 0;
}
