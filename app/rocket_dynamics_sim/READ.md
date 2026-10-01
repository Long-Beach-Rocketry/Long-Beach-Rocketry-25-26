git submodule update --init --recursive
rm -rf build
cmake --preset native
cd build/native
ninja rocket_dynamics_sim
./app/rocket_dynamics_sim/rocket_dynamics_sim