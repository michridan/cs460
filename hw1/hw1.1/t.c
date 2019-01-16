/*********  t.c file *********************/

int prints(char *s)
{
	int i = 0;
	while(s[i])
		putc(s[i++]);
	return i;
}

int gets(char *s)
{
	int i = 0;
	char c = 0;
	while((c = getc()) != '\r')
	{
		putc(c);
		s[i++] = c;
	}
	s[i] = 0;
	return i;
}

char ans[64];

main()
{
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}
