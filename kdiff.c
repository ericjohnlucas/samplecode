#include <stdio.h>
#include <string.h>
    
unsigned N;
unsigned K;

unsigned *parsestringtoint(char *string,int streammode,unsigned *nil)
{
    int i=0;
    unsigned placevalue=1;
    unsigned result=atoi(string);
    if (streammode==0)//n
    {
	N=result;
    }
    else if (streammode==1)//k
    {
        K=result;
    }
    else//append chain
    {
	nil[streammode-3]=result;
    }
    return nil;
}

unsigned *nextline(char *line, int streammode,unsigned *nil)
{
    char word[2000000];
    strcpy(word,line);
    char *pch = strtok (word," ");
    parsestringtoint(pch,streammode,nil);
    streammode++;
    while (pch)
    {
      pch = strtok (NULL," ");
      parsestringtoint(pch,streammode,nil);
      if (streammode==1) break;
      streammode++;
      if (streammode==N+3) break;
    }

    return nil;
}

int binarysearch(unsigned A[], unsigned key, unsigned L, unsigned H)
{
  while (H >= L)
    {
      unsigned M=(L + H)/2;
      if (A[M]< key)
      {
        L=M+1;
      }
      else if (A[M]>key)
      {
        H=M-1;
      }
      else
        return 1;
  }
  return 0;
}

unsigned subcount(unsigned k,unsigned *chain)
{
    unsigned count=0;
    unsigned i=0;
    for (i=0;i<N;i++)
    {
	if (binarysearch(chain,chain[i]+K,i,N-1))
	{
	  count++;
	}
    }
    return count;
}

unsigned *mergesort(unsigned lo, unsigned hi,unsigned *nil)
{
    if (lo<hi)
    {
        unsigned m=(lo+hi)/2;
        nil=mergesort(lo, m,nil);
        nil=mergesort(m+1, hi,nil);
	unsigned i, j, k;
	unsigned b[N];
	for (i=lo; i<=hi; i++)
	{
	    b[i]=nil[i];
	}
	i=lo; j=m+1; k=lo;
	while (i<=m && j<=hi)
	{
	    if (b[i]<=b[j])
	    {
		nil[k++]=b[i++];
	    }
	    else
	    {
		nil[k++]=b[j++];
	    }
	}
	while (i<=m) 
	{
	  nil[k++]=b[i++];
	}
    }
    return nil;
}



void testprint(unsigned *nil)
{
    unsigned i;
    for (i=0;i<N;i++)
    {
	printf("%d\n",nil[i]);
    }
}

int main(int argc, char *argv[])
{
	FILE *infile=fopen(argv[1], "r");
        struct numlist *chain;
	unsigned *nil;
	char word[2000000];
	char word2[2000000];
        fgets(word,2000000,infile);
        nil=nextline(word,0,nil);
        fgets(word2,2000000,infile);
	unsigned nums[N];
        nil=nextline(word2,3,nums);
	nil=mergesort(0,N-1,nil);
        printf("%d\n",subcount(K,nil));
        return 0;
}
