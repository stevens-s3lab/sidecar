// RUN: mkdir -p %T/a-long-directory-name-to-test-allocations-for-exceptions-in-_dl_lookup_symbol_x-since-glibc-2.27
// RUN: %clangxx_asan -g %s -o %T/long-object-path
// RUN: /home/kleftog/sidecar-ae/sidecar/sidecar-monitors/sideasan/x86-64/monitor & %run %T/a-*/../a-*/../a-*/../a-*/../a-*/../a-*/../a-*/../a-*/../long-object-path

int main(void) {
    return 0;
}
