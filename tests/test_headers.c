#include <rift/rift.h>
#include <rift/riftlang.h>
#include <rift/rifttest.h>
#include <rift/riftbridge.h>

int main(void) {
    RIFTToken token = rift_token_create(RIFT_TOKEN_IDENTIFIER, "header");
    rift_token_destroy(&token);
    return 0;
}
