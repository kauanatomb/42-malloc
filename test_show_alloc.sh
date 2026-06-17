#!/bin/bash

echo "╔════════════════════════════════════════════════════════════╗"
echo "║            Testing show_alloc_mem() directly               ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

cat > /tmp/test_show_alloc.c << 'EOF'
#include <stdlib.h>
#include <stdio.h>

extern void show_alloc_mem(void);

int main(void)
{
	printf("Allocate TINY (50 bytes)...\n");
	void *tiny = malloc(50);
	
	printf("Allocate SMALL (500 bytes)...\n");
	void *small = malloc(500);
	
	printf("Allocate LARGE (100000 bytes)...\n");
	void *large = malloc(100000);
	
	printf("\n--- Call show_alloc_mem() ---\n\n");
	show_alloc_mem();
	
	printf("\nClean memory...\n");
	free(tiny);
	free(small);
	free(large);
	
	printf("\n--- Call show_alloc_mem() after free() ---\n\n");
	show_alloc_mem();
	
	return 0;
}
EOF

HOSTTYPE=$(uname -m)_$(uname -s)
echo "HOSTTYPE = $HOSTTYPE"
LIB=$(pwd)/libft_malloc_${HOSTTYPE}.so
echo "Compiling with direct link to $LIB..."
cc -Wall -Wextra -Werror /tmp/test_show_alloc.c "$LIB" -o /tmp/test_show_alloc

echo ""
echo "Running..."
/tmp/test_show_alloc

echo ""
echo "✅ Done!"
