#ifndef CIGARETTE_SMOKERS_H
#define CIGARETTE_SMOKERS_H

#include <dispatch/dispatch.h>


typedef struct {
    int tobacco_count;
    int paper_count;
    int matches_count;
    dispatch_semaphore_t agentSem;
    dispatch_semaphore_t tobaccoSem;
    dispatch_semaphore_t paperSem;
    dispatch_semaphore_t matchesSem;
} cigarette_smokers_t;

void cigarette_smokers_init(cigarette_smokers_t *cs);

void agent_provide_tobacco_paper(cigarette_smokers_t *cs);

void agent_provide_tobacco_matches(cigarette_smokers_t *cs);

void agent_provide_paper_matches(cigarette_smokers_t *cs);

void tobacco_smoker_smoke(cigarette_smokers_t *cs);

void paper_smoker_smoke(cigarette_smokers_t *cs);

void matches_smoker_smoke(cigarette_smokers_t *cs);

void agent_signal(cigarette_smokers_t *cs);

#endif /* CIGARETTE_SMOKERS_H */
