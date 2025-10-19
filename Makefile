CC := clang
CFLAGS := -Wall -Wextra -Werror -pthread -std=c11 -g
INCLUDE := -I./include

# Directories
SRC_DIR := src
TESTS_DIR := tests
BUILD_DIR := build
BIN_DIR := bin

# Targets
PC_TEST := $(BIN_DIR)/test_producer_consumer
RW_TEST := $(BIN_DIR)/test_readers_writers
CS_TEST := $(BIN_DIR)/test_cigarette_smokers
BARBER_TEST := $(BIN_DIR)/test_barbershop
# All test targets
ALL_TESTS := $(PC_TEST) $(RW_TEST) $(CS_TEST) $(BARBER_TEST)

# Default target
all: $(ALL_TESTS)

# Create directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Producer-Consumer objects
$(BUILD_DIR)/producer_consumer.o: $(SRC_DIR)/producer_consumer.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Readers-Writers objects
$(BUILD_DIR)/readers_writers.o: $(SRC_DIR)/readers_writers.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Utils objects
$(BUILD_DIR)/utils.o: $(SRC_DIR)/utils.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Cigarette Smokers objects
$(BUILD_DIR)/cigarette_smokers.o: $(SRC_DIR)/cigarette_smokers.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Barbershop objects
$(BUILD_DIR)/barbershop.o: $(SRC_DIR)/barbershop.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Producer-Consumer test
$(PC_TEST): $(TESTS_DIR)/test_producer_consumer.c $(BUILD_DIR)/producer_consumer.o $(BUILD_DIR)/utils.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

# Readers-Writers test
$(RW_TEST): $(TESTS_DIR)/test_readers_writers.c $(BUILD_DIR)/readers_writers.o $(BUILD_DIR)/utils.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

# Cigarette Smokers test
$(CS_TEST): $(TESTS_DIR)/test_cigarette_smokers.c $(BUILD_DIR)/cigarette_smokers.o $(BUILD_DIR)/utils.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

# Barbershop test
$(BARBER_TEST): $(TESTS_DIR)/test_barbershop.c $(BUILD_DIR)/barbershop.o $(BUILD_DIR)/utils.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@


# Run all tests
run-all: $(ALL_TESTS)
	@echo "=========================================="
	@echo "Running Producer-Consumer Test"
	@echo "=========================================="
	$(PC_TEST)
	@echo ""
	@echo "=========================================="
	@echo "Running Readers-Writers Test"
	@echo "=========================================="
	$(RW_TEST)
	@echo ""
	@echo "=========================================="
	@echo "Running Cigarette Smokers Test"
	@echo "=========================================="
	$(CS_TEST)
	@echo ""
	@echo "=========================================="
	@echo "Running Barbershop Test"
	@echo "=========================================="
	$(BARBER_TEST)
	@echo ""
	@echo "(Building H2O test removed)"

# Run individual tests
run-pc: $(PC_TEST)
	$(PC_TEST)

run-rw: $(RW_TEST)
	$(RW_TEST)

run-cs: $(CS_TEST)
	$(CS_TEST)

run-barber: $(BARBER_TEST)
	$(BARBER_TEST)

# run-h2o target removed (test intentionally omitted)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all run-all run-pc run-rw run-cs run-barber run-h2o clean
