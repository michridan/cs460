int tswitch();

int P(SEMAPHORE *s)
{
    int sr = int_off();
    s->value--;
    if(s->value < 0)
    {
        block(s);
        kprintf("Proc %d blocked\n", running->pid);
        printList("Semaphore Queue", s->queue);
    }
    int_on(sr);
}

int block(SEMAPHORE *s)
{
    running->status = BLOCK;
    enqueue(&s->queue, running);
    tswitch();
}

int V(SEMAPHORE *s)
{
    int sr = int_off(), pid;
    s->value++;
    if(s->value <= 0)
    {
        pid = signal(s);
        kprintf("Proc %d unblocked\n", pid);
        printList("Semaphore Queue", s->queue);
        printList("ReadyQueue", readyQueue);
    }
    int_on(sr);
}

int signal(SEMAPHORE *s)
{
    PROC *p = dequeue(&s->queue);
    p->status = READY;
    enqueue(&readyQueue, p);
    return p->pid;
}

int sleep(int event)
{
  int sr = int_off();
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  tswitch();
  int_on(sr);
}

int wakeup(int event)
{
  PROC *temp, *p;
  temp = 0;
  int sr = int_off();
  
  while (p = dequeue(&sleepList))
  {
    if (p->event == event)
    {
	    p->status = READY;
	    enqueue(&readyQueue, p);
    }
    else
    {
	    enqueue(&temp, p);
    }
  }
  sleepList = temp;
  int_on(sr);
}

int ksleep(int event)
{
  int sr = int_off();
  printf("proc %d going to sleep on event=%d\n", running->pid, event);

  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printList("sleepList", sleepList);
  tswitch();
  int_on(sr);
}

int kwakeup(int event)
{
  PROC *temp, *p;
  temp = 0;
  int sr = int_off();
  
  printList("sleepList", sleepList);

  while (p = dequeue(&sleepList))
  {
    if (p->event == event)
    {
	    printf("wakeup %d\n", p->pid);
	    p->status = READY;
	    enqueue(&readyQueue, p);
    }
    else
    {
	    enqueue(&temp, p);
    }
  }
  sleepList = temp;
  printList("sleepList", sleepList);
  int_on(sr);
}

int kexit(int exitValue)
{	
  PROC *p = running->child, *pnext = p;
	if(running->pid == 1)
	{
		printf("P1 cannot die\n");
		return 0;
	}
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  while(p)
  {
	  pnext = p->sibling;
		p->parent = &proc[1];
    p->ppid = 1;
	 	p->sibling = proc[1].child;
	 	proc[1].child = p;
	 	p = pnext;
	}
  kwakeup(running->parent);
  tswitch();
}

int bury(PROC *pdead)
{
  pdead->status = FREE;
  enqueue(freeList, pdead);
}

int kwait(int *status)
{
  PROC *p = running->child, *pnext, *pprev;
  while(1)
  {
    if(!p)
    {
      kprintf("proc%d has no children!\n", running->pid);
      return -1;
    }
    else if(p->status == ZOMBIE)
    {
      running->child = p->sibling;
      *status = p->pid;
      p->status = FREE;
      p->ppid = 0;
      enqueue(freeList, p);
      return p->pid;
    }

    while(p)
    {
      pprev = p;
      p = p->sibling;
      if(p->status == ZOMBIE)
      {
        pprev->sibling = p->sibling;
        *status = p->pid;
        p->status = FREE;
        p->ppid = 0;
        enqueue(freeList, p);
        return p->pid;
      }
    }

    ksleep(running);
    p = running->child;
  }
}
  
int show_buffer()
{
  BUFFER *p = &buffer;
  int i;
  kprintf("------------ BF -----------------\n");
  kprintf("room=%d data=%d buf=", p->room.value, p->data.value);
  for (i=0; i<p->data.value; i++)
    kputc(p->buf[p->tail+i]);
  kprintf("\n");
  kprintf("----------------------------------\n");
}

int buffer_init()
{
  int i;
  BUFFER *p = &buffer;
  p->head = p->tail = 0;
  p->data.value = 0;      p->data.queue = 0;
  p->room.value = BSIZE;  p->room.queue = 0;
  p->mutex.value = 1;     p->mutex.queue = 0;
}

int produce(char c)
{
  BUFFER *p = &buffer;
  P(&p->room);
  P(&p->mutex);
  p->buf[p->head++] = c;
  p->head %= BSIZE;
  V(&p->mutex);
  V(&p->data);
}

int consume()
{
  int c;
  BUFFER *p = &buffer;
  P(&p->data);
  P(&p->mutex);
  c = p->buf[p->tail++];
  p->tail %= BSIZE;
  V(&p->mutex);
  V(&p->room);
  return c;
}
 
int consumer()
{
  char line[128];
  int nbytes, n, i;

  printf("proc %d as consumer\n", running->pid);
 
  while(1){
    printf("input nbytes to read : " );
    //gets(line);
    //line[strlen(line)-1] = 0;
    //sscanf(line, "%d", &nbytes);
    nbytes = geti();
    show_buffer();
    for (i=0; i<nbytes; i++){
       line[i] = consume();
       printf("%c", line[i]);
    }
    printf("\n");
    show_buffer();
    printf("consumer %d got n=%d bytes : line=%s\n", running->pid, n, line);
  }
}

int producer()
{
  char line[128];
  int nbytes, n, i;

  printf("proc %d as producer\n", running->pid);

  while(1){
    printf("input a string to produce : " );
    
    gets(line);
    line[strlen(line)-1] = 0;

    nbytes = strlen(line);
    printf("nbytes=%d line=%s\n", nbytes, line);
    show_buffer();
    for (i=0; i<nbytes; i++){
      produce(line[i]);
    }
    show_buffer();
    printf("producer %d put n=%d bytes\n", running->pid, nbytes);
  }
}
