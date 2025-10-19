#include "common.h"
#include "cigarette_smokers.h"

void cigarette_smokers_init(cigarette_smokers_t *cs) {
    cs->tobacco_count = 0;
    cs->paper_count = 0;
    cs->matches_count = 0;
    cs->agentSem = dispatch_semaphore_create(1);
    cs->tobaccoSem = dispatch_semaphore_create(0);
    cs->paperSem = dispatch_semaphore_create(0);
    cs->matchesSem = dispatch_semaphore_create(0);
}

void agent_provide_tobacco_paper(cigarette_smokers_t *cs) {
    THREAD_SAFE_PRINT("[AGENT] Providing tobacco and paper → matches smoker\n");
    cs->tobacco_count = 1;
    cs->paper_count = 1;
    dispatch_semaphore_signal(cs->matchesSem);
}

void agent_provide_tobacco_matches(cigarette_smokers_t *cs) {
    THREAD_SAFE_PRINT("[AGENT] Providing tobacco and matches → paper smoker\n");
    cs->tobacco_count = 1;
    cs->matches_count = 1;
    dispatch_semaphore_signal(cs->paperSem);
}

void agent_provide_paper_matches(cigarette_smokers_t *cs) {
    THREAD_SAFE_PRINT("[AGENT] Providing paper and matches → tobacco smoker\n");
    cs->paper_count = 1;
    cs->matches_count = 1;
    dispatch_semaphore_signal(cs->tobaccoSem);
}

void tobacco_smoker_smoke(cigarette_smokers_t *cs) {
    dispatch_semaphore_wait(cs->tobaccoSem, DISPATCH_TIME_FOREVER);
    THREAD_SAFE_PRINT("[SMOKER-TOBACCO] Got paper and matches, now smoking with my tobacco...\n");
    cs->paper_count = 0;
    cs->matches_count = 0;
    sleep(1);
    THREAD_SAFE_PRINT("[SMOKER-TOBACCO] Finished smoking!\n");
    dispatch_semaphore_signal(cs->agentSem);
}

void paper_smoker_smoke(cigarette_smokers_t *cs) {
    dispatch_semaphore_wait(cs->paperSem, DISPATCH_TIME_FOREVER);
    THREAD_SAFE_PRINT("[SMOKER-PAPER] Got tobacco and matches, now smoking with my paper...\n");
    cs->tobacco_count = 0;
    cs->matches_count = 0;
    sleep(1);
    THREAD_SAFE_PRINT("[SMOKER-PAPER] Finished smoking!\n");
    dispatch_semaphore_signal(cs->agentSem);
}

void matches_smoker_smoke(cigarette_smokers_t *cs) {
    dispatch_semaphore_wait(cs->matchesSem, DISPATCH_TIME_FOREVER);
    THREAD_SAFE_PRINT("[SMOKER-MATCHES] Got tobacco and paper, now smoking with my matches...\n");
    cs->tobacco_count = 0;
    cs->paper_count = 0;
    sleep(1);
    THREAD_SAFE_PRINT("[SMOKER-MATCHES] Finished smoking!\n");
    dispatch_semaphore_signal(cs->agentSem);
}

void agent_signal(cigarette_smokers_t *cs) {
    dispatch_semaphore_wait(cs->agentSem, DISPATCH_TIME_FOREVER);
}
