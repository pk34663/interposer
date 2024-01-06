#define _GNU_SOURCE
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>

typedef struct action {
    char *name;
} action_t;

int main(void)
{
    std::string s = "abc";
    std::map<std::string,std::vector<action_t *> > m;
    action_t *a = (action_t *)malloc(sizeof(action_t));

    m[s].push_back(a);
}
