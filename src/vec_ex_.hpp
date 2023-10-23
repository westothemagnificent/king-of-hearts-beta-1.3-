//vec_ex
#include<iostream>
#include<string>
#include<vector>

namespace vec_ex {
    template <typename T>
    std::vector<T> add_vec_to_vec(std::vector<T> vec1, std::vector<T> vec2) {
        for(int i=0; i < size(vec2); i++){
            vec1.push_back(vec2[i]);
        }
        return vec1;
    }
    template <typename T>
    int find_gratest_in_vec(std::vector<T> vec) {
        int score;
        for (int i = 0; i < vec.size(); i++) {
            score = 0;
            for (int i2 = 0; i2 < vec.size(); i2++) {
                if (vec[i] >= vec[i2]) {
                    score += 1;
                }
            }
            if(score == size(vec)){
                    return i;
                }
        }
        return 0;
    }
    template <typename T>
    int find_least_in_vec(std::vector<T> vec) {
        int score;
        for (int i = 0; i < vec.size(); i++) {
            score = 0;
            for (int i2 = 0; i2 < vec.size(); i2++) {
                if (vec[i] <= vec[i2]) {
                    score += 1;
                }
            }
            if(score == size(vec)){
                    return i;
                }
        }
        return 0;
    }
    template <typename T>
    int find_char(T find, std::vector<T> vec) {
        for (int i = 0; i < vec.size(); i++) {
            if (vec[i] == find) {
                return i;
            }
        }
        return -1;
    }
    template <typename T>
    int find_char(T find, std::pair<T, T> vec) {
        if(vec.first == find){
            return 1;
        }
        else{
            if(vec.second == find){
                return 2;
            }
        }
        return -1;
    }
    template <typename T>
    void print_vec(std::vector<T> vec) {
        for (int i = 0; i < vec.size(); i++) {
            std::cout << ", " << vec[i];
        }
        std::cout << "\n";
    }
    template <typename T>
    void print_vec(std::vector<T> vec, int disnum) {
        for (int i = 0; i < vec.size(); i++) {
            std::cout << i  << ". " << vec[i] << "\n";
        }
    }
    template <typename T>
    std::vector<T> copy_array_to_vec(T arr[], int l) {
        std::vector<T> vector_to_add;
        for (int i = 0; i < l; i++) {
           vector_to_add.push_back(arr[i]);
        }
        return vector_to_add;
    }
    

    int sum_oanbi_vec(std::vector<int> list_to_sum_to, int start){
        int sum = 0;
        for(int i=0; i <= start; i++){
            sum += list_to_sum_to[i];
        }
        return sum;
    }


    namespace arr_ex{
    template <typename T>
    int print_arr(T arr[], int l) {
        for (int i = 0; i < l; i++) {
            std::cout << arr[i] << "\n";
        }
        return -1;
    }
}
}

