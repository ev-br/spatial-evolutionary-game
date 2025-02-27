#include <iostream>
#include "evolve.h"
#include <vector>
#include <chrono>
#include <cstdint>
#include "cnpy.h"


/*
 * Compilation require cnpy lib
 * export LD_LIBRARY_PATH=/usr/local/lib/
 * g++ -o build/realization.out main.cpp -L/usr/local/lib evolve.cc -lcnpy -lz -O3 --std=c++11
 */
using namespace std;

const int total_steps = 18000;
const int drop_steps = 10000;
const int L = 100;
const int fields_number = 40;

vector<int> get_field(int num){
    stringstream ss;
    ss << "fields/field_" << L << "_" << num << ".npy";
    cnpy::NpyArray arr = cnpy::npy_load(ss.str());
    int64_t* loaded_data = arr.data<int64_t>();
    vector<int> field;
    for(int i = 0; i < L*L; i++){
        field.push_back(static_cast<int>(loaded_data[i]));
    }
    return field;
}


int main() {
    cout << "Initializing variables..." << endl;
    vector<double> bs = {0.9       , 0.91864407, 0.93728814, 0.9559322 , 0.97457627,
                         0.99322034, 1.01186441, 1.03050847, 1.04915254, 1.06779661,
                         1.08644068, 1.10508475, 1.12372881, 1.14237288, 1.16101695,
                         1.17966102, 1.19830508, 1.21694915, 1.23559322, 1.25423729,
                         1.27288136, 1.29152542, 1.31016949, 1.32881356, 1.34745763,
                         1.36610169, 1.38474576, 1.40338983, 1.4220339 , 1.44067797,
                         1.45932203, 1.4779661 , 1.49661017, 1.51525424, 1.53389831,
                         1.55254237, 1.57118644, 1.58983051, 1.60847458, 1.62711864,
                         1.64576271, 1.66440678, 1.68305085, 1.70169492, 1.72033898,
                         1.73898305, 1.75762712, 1.77627119, 1.79491525, 1.81355932,
                         1.83220339, 1.85084746, 1.86949153, 1.88813559, 1.90677966,
                         1.92542373, 1.9440678 , 1.96271186, 1.98135593, 1.995};
    MeanGame game(L);
    vector<double> densities, new_den;
    chrono::duration<double> elapsed;
    cout << "Modeling..." << endl;
    for(int i = 0; i < bs.size(); i++){
        game.set_b(bs[i]);
        auto start = chrono::high_resolution_clock::now();
        for(int j = 0; j < fields_number; j++){
            game.set_field(get_field(j));
            game.evolve(total_steps/2);
            game.evolve(total_steps - total_steps/2);
            new_den = game.get_densities();
            densities.insert(densities.end(), new_den.begin()+drop_steps+1, new_den.end());
            new_den.clear();
        }

        auto finish = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = finish - start;
        cout << i+1 << "\\" << bs.size() << endl
             << "\tOne round in " << elapsed.count() << " s" << endl
             << " Time remaining: " << elapsed.count()*(bs.size()-i-1) << " s" << endl;
        cout << "Saving data: ";

        cnpy::npy_save("data/density_of_cooperators5.npy",
                       &densities[0],
                       {i+1, fields_number, total_steps-drop_steps},
                       "w");
        cout << "done" << endl;
    }
    cnpy::npy_save("data/density_of_cooperators5.npy",
                   &densities[0],
                   {bs.size(), fields_number, total_steps-drop_steps},
                   "w");
    return 0;
}