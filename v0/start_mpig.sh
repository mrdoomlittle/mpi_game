mpicxx -Wall -std=c++14 mpi_game/mpi_game.cpp -lboost_system -lboost_filesystem -lpthread -lboost_thread -o mpi_game/mpi_game.exec

scp -r mpi_game 192.168.0.100:/home/daniel-robson
scp -r mpi_game 192.168.0.101:/home/daniel-robson

mpicxx -Wall -std=c++14 -DMASTER_NODE mpi_game/mpi_game.cpp -lboost_system -lboost_filesystem -lpthread -lboost_thread -lsfml-graphics -lsfml-window -lsfml-system  -I/usr/local/cuda-8.0/include -L/usr/local/cuda-8.0/lib64 -o mpi_game/mpi_game.exec

export MPIEXEC_PORT_RANGE="4000:4100";

mpiexec -f mpi_game/machines -n 17 ./mpi_game/mpi_game.exec
