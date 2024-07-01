// Created by jordan on 30/06/24.

#include "PyrPrimitive.h"
#include "VMGlobals.h"
#include "PyrObject.h"
#include "SC_LanguageClient.h"
#include "git2.h"


int prGitInit(struct VMGlobals* g, int numArgsPushed) {
    git_libgit2_init();
    return errNone;
}

int prGitClone(struct VMGlobals* g, int numArgsPushed) {
    // PyrSlot* gitObj = g->sp - 2;
    PyrSlot* url = g->sp - 1;
    PyrSlot* localPath = g->sp;

    if (!IsObj(url) && slotRawClass(url) != class_string)
        return errWrongType;
    if (!IsObj(localPath) && slotRawClass(localPath) != class_string)
        return errWrongType;

    PyrString* urlO = slotRawString(url);
    PyrString* localPathO = slotRawString(localPath);

    std::string urlStr(&urlO->s[0], &urlO->s[urlO->size]);
    std::string localPathStr(&localPathO->s[0], &localPathO->s[localPathO->size]);

    git_repository* repo;
    if (git_clone(&repo, urlStr.c_str(), localPathStr.c_str(), nullptr) != 0) {
        post("Git err: %s\n", git_error_last()->message);
        return errFailed;
    }

    return errNone;
}

int prGitShutdown(struct VMGlobals* g, int numArgsPushed) {
    git_libgit2_shutdown();
    return errNone;
}

void initGitPrimitives() {
    int base = nextPrimitiveIndex(), index = 0;
    definePrimitive(base, index++, "_GitInit", prGitInit, 1, 0);
    definePrimitive(base, index++, "_GitClone", prGitClone, 3, 0);
}
