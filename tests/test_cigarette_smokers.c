#include "common.h"
#include "cigarette_smokers.h"

typedef struct {
    cigarette_smokers_t *cs;
    int round;
} agent_args_t;

typedef struct {
    cigarette_smokers_t *cs;
    int id;
} smoker_args_t;

void *agent_thread(void *arg) {
    agent_args_t *args = (agent_args_t *)arg;
    for (int round = 0; round < 3; round++) {
        agent_signal(args->cs);
        int choice = round % 3;
        switch (choice) {
            case 0: agent_provide_tobacco_paper(args->cs); break;
            case 1: agent_provide_tobacco_matches(args->cs); break;
            case 2: agent_provide_paper_matches(args->cs); break;
        }
    }
    free(args);
    return NULL;
}

void *tobacco_smoker_thread(void *arg) {
    smoker_args_t *args = (smoker_args_t *)arg;
    tobacco_smoker_smoke(args->cs);
    free(args);
    return NULL;
}

void *paper_smoker_thread(void *arg) {
    smoker_args_t *args = (smoker_args_t *)arg;
    paper_smoker_smoke(args->cs);
    free(args);
    return NULL;
}

void *matches_smoker_thread(void *arg) {
    smoker_args_t *args = (smoker_args_t *)arg;
    matches_smoker_smoke(args->cs);
    free(args);
    return NULL;
}

int main() {
    cigarette_smokers_t cs;
    cigarette_smokers_init(&cs);
    pthread_t agent, tobacco_smoker, paper_smoker, matches_smoker;

    agent_args_t *agent_args = malloc(sizeof(agent_args_t));
    agent_args->cs = &cs;
    pthread_create(&agent, NULL, agent_thread, agent_args);

    smoker_args_t *tobacco_args = malloc(sizeof(smoker_args_t));
    tobacco_args->cs = &cs;
    pthread_create(&tobacco_smoker, NULL, tobacco_smoker_thread, tobacco_args);

    smoker_args_t *paper_args = malloc(sizeof(smoker_args_t));
    paper_args->cs = &cs;
    pthread_create(&paper_smoker, NULL, paper_smoker_thread, paper_args);

    smoker_args_t *matches_args = malloc(sizeof(smoker_args_t));
    matches_args->cs = &cs;
    pthread_create(&matches_smoker, NULL, matches_smoker_thread, matches_args);

    pthread_join(agent, NULL);
    pthread_join(tobacco_smoker, NULL);
    pthread_join(paper_smoker, NULL);
    pthread_join(matches_smoker, NULL);

    return EXIT_SUCCESS;
}
