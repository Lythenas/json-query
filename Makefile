CPP = clang++
CPPFLAGS = -std=c++20 -Wall -Wextra

.PHONY: jsonquery

jsonquery: Makefile
	$(CPP) $(CPPFLAGS) src/jsonquery.cpp -o jsonquery
