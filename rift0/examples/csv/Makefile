# Advanced CSV Examples Makefile with R"" syntax support
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Werror -I../../include -g -O2
LDFLAGS = -lm -pthread

# Library linking (prefer static)
LIB_PATH = ../../lib/librift0.a
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.c)
TARGETS = $(SOURCES:$(SRCDIR)/%.c=%)

.PHONY: all clean test benchmark data

all: $(TARGETS)

# Compile each example
%: $(SRCDIR)/%.c $(LIB_PATH)
	@echo "🔨 Building CSV example: $@"
	$(CC) $(CFLAGS) $< $(LIB_PATH) $(LDFLAGS) -o $@

# Ensure data files exist
data:
	@echo "📁 Checking data files..."
	@test -f ../data/simple.csv || (echo "❌ Missing ../data/simple.csv" && exit 1)
	@test -f ../data/complex.csv || (echo "❌ Missing ../data/complex.csv" && exit 1)
	@echo "✅ Data files ready"

# Test all examples
test: all data
	@echo "🧪 Testing Advanced CSV Examples with R\"\" syntax"
	@echo "================================================="
	@echo ""
	@for target in $(TARGETS); do \
		echo "🎯 Running $$target..."; \
		echo "========================"; \
		./$$target; \
		echo ""; \
	done

# Run benchmark specifically  
benchmark: csv_benchmark data
	@echo "⚡ Running CSV Benchmark..."
	./csv_benchmark

# Clean compiled examples
clean:
	@echo "🧹 Cleaning CSV examples..."
	rm -f $(TARGETS)

# Build library if needed
$(LIB_PATH):
	@echo "📚 Building required library..."
	@cd ../.. && make lib

help:
	@echo "🎯 CSV Examples Build System"
	@echo "============================"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build all CSV examples"
	@echo "  test      - Run all examples"
	@echo "  benchmark - Run performance benchmark"
	@echo "  data      - Check data files"
	@echo "  clean     - Clean compiled examples"
	@echo ""
	@echo "Examples:"
	@echo "  make test      # Run all R\"\" syntax demos"
	@echo "  make benchmark # Performance testing"
