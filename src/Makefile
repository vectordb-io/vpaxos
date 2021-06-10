CC:=g++
CFLAGS+=-c -Wall -g -std=c++17
INCLUDES=-I./ -I../third_party/jsonxx.v0.9.2/include/

OBJS_SERVER=server_main.o config.o vpaxos_rpc.grpc.pb.o vpaxos_rpc.pb.o status.o thread_pool.o timer.o grpc_server.o env.o node.o proposer.o acceptor.o learner.o storage.o coding.o async_task_called.o async_task_call.o async_req_manager.o
INCLUDES_SERVER=-I./
LIBS_SERVER=-lglog -lprotobuf -lgrpc++ -lgrpc -lpthread -lleveldb
LFLAGS_SERVER=-L/usr/local/lib

OBJS_CLIENT=client_main.o vpaxos_rpc.grpc.pb.o vpaxos_rpc.pb.o status.o
INCLUDES_CLIENT=-I./
LIBS_CLIENT=-lglog -lprotobuf -lgrpc++ -lgrpc -lpthread -lleveldb
LFLAGS_CLIENT=-L/usr/local/lib

OBJS_THREAD_POOL_TEST=thread_pool_test.o thread_pool.o
INCLUDES_THREAD_POOL_TEST=-I./
LIBS_THREAD_POOL_TEST=-lglog -lprotobuf -lgrpc++ -lgrpc -lpthread -lleveldb
LFLAGS_THREAD_POOL_TEST=-L/usr/local/lib

OBJS_NODEID_TEST=nodeid_test.o 
INCLUDES_NODEID_TEST=-I./ -I../third_party/jsonxx.v0.9.2/include/
LIBS_NODEID_TEST=
LFLAGS_NODEID_TEST=

cs: vpaxos_server vpaxos_client
	@echo ""
	@echo "success, go go go!"
	@echo ""

test: thread_pool_test nodeid_test
	@echo ""
	@echo "make $<"
	@echo ""

nodeid_test: $(OBJS_NODEID_TEST)
	$(CC) $^ -o $@ $(LIBS_NODEID_TEST) $(LFLAGS_NODEID_TEST)

thread_pool_test: $(OBJS_THREAD_POOL_TEST)
	$(CC) $^ -o $@ $(LIBS_THREAD_POOL_TEST) $(LFLAGS_THREAD_POOL_TEST)

vpaxos_server: $(OBJS_SERVER)
	$(CC) $^ -o $@ $(LIBS_SERVER) $(LFLAGS_SERVER)

vpaxos_client: $(OBJS_CLIENT)
	$(CC) $^ -o $@ $(LIBS_CLIENT) $(LFLAGS_CLIENT)

%.o: %.cc %.h
	echo "----------------"
	echo $(INCLUDES)
	echo $(CFLAGS)
	echo "----------------"
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ $<

.PHONY:
proto:
	protoc -I. --cpp_out=. vpaxos_rpc.proto
	protoc -I. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` vpaxos_rpc.proto

all:
	make proto && make cs && make test

clean:
	rm -rf *.o 
	rm -rf vpaxos_server
	rm -rf vpaxos_client
	rm -rf thread_pool_test
	rm -rf nodeid_test

cleanall: clean
	rm -f vpaxos_rpc.pb.*
	rm -f vpaxos_rpc.grpc.pb.*




