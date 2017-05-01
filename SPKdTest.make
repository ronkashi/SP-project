CC = gcc
OBJS = kd_unit_test.o SPKDArray.o SPPoint.o SPLogger.o SPKDTree.o SPBPriorityQueue.o
EXEC = kd_unit_test
TESTS_DIR = ./unit_tests
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
kd_unit_test.o: $(TESTS_DIR)/kd_unit_test.c $(TESTS_DIR)/unit_test_util.h  SPKDArray.h SPPoint.h SPKDTree.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c


SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDArray.o: SPKDArray.c SPKDArray.h SPPoint.h SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPKDTree.o: SPKDTree.c SPKDTree.h SPKDArray.h SPPoint.h SPConfig.h SPBPriorityQueue.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)

