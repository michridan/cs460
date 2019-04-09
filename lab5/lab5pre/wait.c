int tswitch();

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
  //printList("sleepList", sleepList);
  tswitch();
  int_on(sr);
}

int kwakeup(int event)
{
  PROC *temp, *p;
  temp = 0;
  int sr = int_off();
  
  //printList("sleepList", sleepList);

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
  //printList("sleepList", sleepList);
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
  printf("%d, %d\n", running->pid, p->pid);
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
      printf("%d\n", running->pid);
      enqueue(freeList, p);
      printf("%d\n", running->pid);
      return p->pid;
    }
    printf("%d", running->pid);

    while(p)
    {
      printf("%d loop\n", running->pid);
      pprev = p;
      p = p->sibling;
      if(p->status == ZOMBIE)
      {
        printf("%d if\n", running->pid);
        pprev->sibling = p->sibling;
        *status = p->pid;
        p->status = FREE;
        p->ppid = 0;
        printf("%d\n", running->pid);
        enqueue(freeList, p);
        printf("%d\n", running->pid);
        return p->pid;
      }
    }

    ksleep(running);
    p = running->child;
  }
}
