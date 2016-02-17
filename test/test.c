#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "err.h"

#define LONG_MAX 2147483647

static int print_test(int n, const char *s);
static int print_maps(void);

static void pr_errno(const char *msg)
{
    printf("error: %s, %s", strerror(errno), msg);
}

static void pr_err(const char *msg)
{
    printf("error: %s", msg);
}

/*
    prints a region with ".........."
*/
static int test_1(void)
{
    pid_t parent;
    int w;

    parent = fork();
    if (parent < 0) {
        printf("Error: test_1, fork %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (!parent) {
        void *new_map = mmap(NULL, 4096*10,
            PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

        if (new_map < 0)
            printf("Error: test_1, new map %s\n", strerror(errno));
        else if (new_map == 0)
            printf("Error: test_1, new map null.\n");
        else
            printf("in test_1 child, pid(%i), new_map(%lx)\n",
                getpid(), (unsigned long)new_map);
        print_maps();
        if (munmap(new_map, 4096*10))
            printf("Error:, test_1, mun %s\n", strerror(errno));
        exit(EXIT_SUCCESS);
    } else {
        w = wait(NULL);
        if (w < 0)
            printf("Error: waiting for test_1, %s\n",
                strerror(errno));
        else
            printf("pid(%i): done waiting for test_1, pid(%i).\n",
                getpid(), w);
        printf("\n");
        return 0;
    }
}

/*
    prints a region with "1111111111"
*/
static int test_2(void)
{
    pid_t parent;
    int w;

    parent = fork();
    if (parent < 0) {
        printf("Error: test_2, fork %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (!parent) {
        char *new_map = mmap(NULL, 4096*10,
            PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (new_map < 0)
            printf("Error: test_2, new map %s\n", strerror(errno));
        else if (new_map == 0)
            printf("Error: test_2, new map null.\n");
        else {
            printf("in test_2 child, pid(%i), new_map(%lx)\n",
                getpid(), (unsigned long)new_map);
            char c = '0';

            while (c-'0' < 10) {
                *new_map = c;
                new_map += 4096;
                c += 1;
            }
        }
        print_maps();
        if (munmap(new_map-4096*10, 4096*10))
            printf("Error:, test_2, mun %s\n", strerror(errno));
        exit(EXIT_SUCCESS);
    } else {
        w = wait(NULL);
        if (w < 0)
            printf("Error: waiting for test_2, %s\n",
                strerror(errno));
        else
            printf("pid(%i): done waiting for test_2, pid(%i).\n",
                getpid(), w);
        printf("\n");
        return 0;
    }
}

/*
    prints a region with ".1.1.1.1.1"
*/
static int test_3(void)
{
    pid_t parent;
    int w;

    parent = fork();
    if (parent < 0) {
        printf("Error: test_3, fork %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (!parent) {
        char *new_map = mmap(NULL, 4096*10,
            PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (new_map < 0)
            printf("Error: test_3, new map %s\n", strerror(errno));
        else if (new_map == 0)
            printf("Error: test_3, new map null.\n");
        else {
            printf("in test_3 child, pid(%i), new_map(%lx)\n",
                getpid(), (unsigned long)new_map);
            char c = '0';

            new_map += 4096;
            while (c-'0' < 5) {
                *new_map = c;
                new_map += 8192;
                c += 1;
            }
        }
        print_maps();
        if (munmap(new_map-4096*9, 4096*10))
            printf("Error:, test_3, mun %s\n", strerror(errno));
        exit(EXIT_SUCCESS);
    } else {
        w = wait(NULL);
        if (w < 0)
            printf("Error: waiting for test_3, %s\n",
                strerror(errno));
        else
            printf("pid(%i): done waiting for test_3, pid(%i).\n",
                getpid(), w);
        printf("\n");
        return 0;
    }
}

/*
    prints a region with "22222....."
*/
static int test_4(void)
{
    pid_t parent, first_child;
    int w;

    parent = fork();
    if (parent < 0) {
        printf("Error: test_4, fork %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (!parent) {
        char *new_map = mmap(NULL, 4096*10,
            PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        if (new_map < 0)
            printf("Error: test_4, new map %s\n", strerror(errno));
        else if (new_map == 0)
            printf("Error: test_4, new map null.\n");
        else {
            printf("in test_4 child, pid(%i), new_map(%lx)\n",
                getpid(), (unsigned long)new_map);
            char c = '0';

            while (c-'0' < 5) {
                *new_map = c;
                new_map += 4096;
                c += 1;
            }
            first_child = fork();
        }
        if (!first_child) {
            print_maps();
            if (munmap(new_map-4096*5, 4096*10))
                printf("Error:, test_4, mun %s\n",
                    strerror(errno));
            exit(EXIT_SUCCESS);
        }
        w = wait(NULL);
        if (w < 0)
            printf("Error: waiting for test_4 grandchild, %s\n",
                strerror(errno));
        else
            printf("pid(%i): done waiting for test_4, pid(%i).\n",
                getpid(), w);
        exit(EXIT_SUCCESS);
    } else {
        w = wait(NULL);
        if (w < 0)
            printf("Error: waiting for test_4, %s\n",
                strerror(errno));
        else
            printf("pid(%i): done waiting for test_4, pid(%i).\n",
                getpid(), w);
        printf("\n");
        return 0;
    }
}

/*
    prints a region with "1111..2222"
*/
static int test_5(void)
{
    pid_t parent, first_child;
    int w;

    parent = fork();
    if (parent < 0) {
        printf("Error: test_5, fork %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (!parent) {
        char *new_map = mmap(NULL, 4096*10,
            PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

        if (new_map < 0)
            printf("Error: test_5, new map %s\n", strerror(errno));
        else if (new_map == 0)
            printf("Error: test_5, new map null.\n");
        else {
            printf("in test_5 child, pid(%i), new_map(%lx)\n",
                getpid(), (unsigned long)new_map);
            char c = '0';

            new_map += 4096*6;
            while (c-'0' < 4) {
                *new_map = c;
                new_map += 4096;
                c += 1;
            }
            first_child = fork(); /* second, grandchild */
        }
        if (!first_child) { /* second, grandchild */
            new_map -= 4096*10;
            char c = '0';

            while (c-'0' < 4) {
                *new_map = c;
                new_map += 4096;
                c += 1;
            }
            print_maps();
            if (munmap(new_map-4096*5, 4096*10))
                printf("Error:, test_5, mun %s\n",
                    strerror(errno));
            exit(EXIT_SUCCESS);
        }
        w = wait(NULL);
        if (w < 0)
            printf("Error: waiting for test_5 grandchild, %s\n",
                strerror(errno));
        else
            printf("pid(%i): done waiting for test_5, pid(%i).\n",
                getpid(), w);
        exit(EXIT_SUCCESS);
    } else {
        w = wait(NULL);
        if (w < 0)
            printf("Error: waiting for test_5, %s\n",
                strerror(errno));
        else
            printf("pid(%i): done waiting for test_5, pid(%i).\n",
                getpid(), w);
        printf("\n");
        return 0;
    }
}

/*
    prints a region with 2000*'.'
*/
static int test_6(void)
{
    pid_t parent;
    int w;

    parent = fork();
    if (parent < 0) {
        printf("Error: test_6, fork %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (!parent) {
        void *new_map = mmap(NULL, 4096*2000,
            PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

        if (new_map < 0)
            printf("Error: test_6, %s\n", strerror(errno));
        else if (new_map == 0)
            printf("Error: test_6, new map null.\n");
        else
            printf("in test_6 child, pid(%i), new_map(%lx)\n",
                getpid(), (unsigned long)new_map);
        print_maps();
        if (munmap(new_map, 4096*2000))
            printf("Error:, test_6, mun %s\n", strerror(errno));
        exit(EXIT_SUCCESS);
    } else {
        w = wait(NULL);
        if (w < 0)
            printf("Error: waiting for test_6, %s\n",
                strerror(errno));
        else
            printf("pid(%i): done waiting for test_6, pid(%i).\n",
                getpid(), w);
        printf("\n");
        return 0;
    }
}

/*
    triggers Linux OOM
*/
static int test_7(void)
{
    pid_t parent;
    int w, status;

    parent = fork();
    if (parent < 0) {
        printf("Error: test_7, fork %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int printed_once = 0;

    while (!parent) {
        char *new_map = mmap(NULL, LONG_MAX,
            PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

        if (new_map < 0)
            printf("Error: test_7, %s\n", strerror(errno));
        else if (new_map == 0)
            printf("Error: test_7, new map null.\n");
        else
            printf("in test_7 child, pid(%i), new_map(%lx)\n",
                getpid(), (unsigned long)new_map);

        char *address = new_map;
        char c = '0';

        while (address < new_map + LONG_MAX) {
            *address = c;
            address += 4096;
        }
        if (!printed_once) {
            printf("Printing once:\n");
            print_maps();
            printed_once = 1;
        }
    }

    if (!parent)
        exit(EXIT_SUCCESS);
    else {
        w = wait(&status);
        if (w < 0)
            printf("Error: waiting for test_7, %s\n",
                strerror(errno));
        else {
            printf("pid(%i): done waiting for test_7, pid(%i).\n",
                getpid(), w);
            printf("WIFEXITED:%i\n", WIFEXITED(status));
            printf("WEXITSTATUS:%i\n", WEXITSTATUS(status));
            printf("WIFSIGNALED:%i\n", WIFSIGNALED(status));
            printf("WTERMSIG:%i\n", WTERMSIG(status));
            printf("WCOREDUMP:%i\n", WCOREDUMP(status));
            printf("WIFSTOPPED:%i\n", WIFSTOPPED(status));
            printf("WSTOPSIG:%i\n", WSTOPSIG(status));
            printf("WIFCONTINUED:%i\n", WIFCONTINUED(status));
        }
        printf("\n");
        return 0;
    }
}

int main(int argc, char **argv)
{
    printf("main: pid(%i)\n", getpid());
    print_maps();

    print_test(1, "..........");
    test_1();

    print_test(2, "1111111111");
    test_2();

    print_test(3, ".1.1.1.1.1");
    test_3();

    print_test(4, "22222.....");
    test_4();

    print_test(5, "1111..2222");
    test_5();

    print_test(6, "2000 '.'");
    test_6();

    print_test(7, "trigger OOM");
    test_7();

    print_maps();
    return 0;
}

static int print_test(int n, const char *s)
{
    return printf("\nTest %d, %s\n\n", n, s);
}

static int print_maps(void)
{
    /*
     * You may not modify print_maps().
     * Every test should call print_maps() once.
     */
    char *path;
    char str[25000];
    int fd;
    int r, w;

    path = "/proc/self/maps";
    printf("%s:\n", path);

    fd = open(path, O_RDONLY);

    if (fd < 0)
        pr_errno(path);

    r = read(fd, str, sizeof(str));

    if (r < 0)
        pr_errno("cannot read the mapping");

    if (r == sizeof(str))
        pr_err("mapping too big");

    while (r) {
        w = write(1, str, r);
        if (w < 0)
            pr_errno("cannot write to stdout");
        r -= w;
    }

    return 0;
}
