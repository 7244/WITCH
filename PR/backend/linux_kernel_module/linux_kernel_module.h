void PR_exit(uint32_t num) {
  /* best implement ever */
  while(1);
}

void PR_abort(void) {
  panic("[PR] PR_abort()");
}

static void _PR_internal_open(){}
static void _PR_internal_close(){}
