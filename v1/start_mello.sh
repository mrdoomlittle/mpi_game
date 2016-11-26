mpicxx -Wall -std=c++14 mello/mello.cpp -lboost_system -lboost_filesystem -lpthread -lboost_thread -lboost_mpi -lboost_serialization -o mello/mello.exec

scp -r mello 192.168.0.100:/home/daniel-robson
scp -r mello 192.168.0.101:/home/daniel-robson

mpicxx -DCORE_NODE -Wall -std=c++14 mello/mello.cpp -lboost_system -lboost_filesystem -lpthread -lboost_thread -lboost_mpi -lboost_serialization -lsfml-graphics -lsfml-window -lsfml-system -o mello/mello.exec

export MPIEXEC_PORT_RANGE="4000:4100";

mpiexec -f mello/machines -n 17 ./mello/mello.exec 120 64 1920 1024 24 24
