#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include "search.h"

/*search.c designed by Eric J D'Souza

A simple Ascii search tool implemented in C
Relies on an index created by the attached file index.c
*/

struct occlist /*linked list of filenames with their corresponding occurences*/
{
	char *doc;
  	struct occlist *next;
};

struct wordlist /*linked list of words; each word also has an occlist which branches off which contains the occurences for each file*/
{
	char *term;
	struct wordlist *next;
	struct occlist *occhead;
};

struct searchquery
{
	char **sq;
	int sq_index;
};

static char *cacherange;

static int cachemode;

int remaining;

void printlist(struct wordlist *p) 
{
	/*prints the index*/
	struct wordlist *temp;
	struct occlist *inside;
	temp = p;
	if(p!= NULL)
	{
		while (temp!= NULL)
		{
			printf("%s ",temp->term);
			inside=temp->occhead;
			while (inside!= NULL)
			{
				printf("%s ",inside->doc);
				inside=inside->next;
			} 
			temp=temp->next;
			printf("\n");
		} 
	}
	else
		printf("THE INDEX IS EMPTY\n");
}

void prePrintpostFree_query(struct occlist *subchain)
{
	/*this method recurses to the end of the occurence list, and frees each link from memory on the way back*/
	if (subchain==NULL)
	{
		return;
	}
	else
	{
		printf("%s\n",subchain->doc);
		prePrintpostFree_query(subchain->next);
		free(subchain->doc);
		free(subchain);
	}
}

int belongsToSublist(char *filename,struct occlist *list)
{
	/*this method iterates throught the list;1 is returned if the filename belongs in the list and 0 is returned it it does not belong in the list*/
	int value=0;
	while(list!=NULL)
	{
		if (strcmp(filename,list->doc)==0)
		{
			value=1;
			break;
		}
		list=list->next;
	}
	return value;
}

void append_occ(struct occlist *querylist,struct occlist *newlist)
{
	/*sets the pointer of the last link of the first list to the head of the second list*/
	if (newlist!=NULL)
	{
		while (querylist!=NULL)
		{
			if (querylist->next==NULL)
			{
				querylist->next=newlist;
				break;
			}
			querylist=querylist->next;
		}
	}
}

struct occlist *sa(struct occlist *querylist,struct wordlist *chain,char *word)
{
	/*implements logical AND search*/
	struct occlist *subchain;
	struct occlist *index;
	int wordfound=0;
	while(chain!=NULL)
	{
		if (strcmp(chain->term,word)==0)
		{
			wordfound=1;
			subchain=chain->occhead;
			if (querylist==NULL)
			{
				while(subchain!=NULL)
				{
					if (querylist==NULL)
					{
						querylist = (struct occlist*) malloc(sizeof(struct occlist));
						querylist->doc = (char *) malloc(strlen(subchain->doc)+1);
						strcpy(querylist->doc,subchain->doc);
						querylist->next=NULL;
						index=querylist;
					}
					else
					{
						struct occlist *new = (struct occlist *) malloc(sizeof(struct occlist));
						new->doc = (char *) malloc(strlen(subchain->doc)+1);
						strcpy(new->doc,subchain->doc);
						new->next=NULL;
						index->next=new;
						index=new;
					}
					subchain=subchain->next;
				}
			}
			else
			{
				struct occlist *newlist=NULL;
				struct occlist *newindex;
				index=querylist;
				while(index!=NULL)
				{			
					if (belongsToSublist(index->doc,subchain)==1)
					{
						if (newlist==NULL)
						{
							newlist = (struct occlist*) malloc(sizeof(struct occlist));
							newlist->doc = (char *) malloc(strlen(index->doc)+1);
							strcpy(newlist->doc,index->doc);
							newlist->next=NULL;
							newindex=newlist;
						}
						else
						{
							struct occlist *new = (struct occlist *) malloc(sizeof(struct occlist));
							new->doc = (char *) malloc(strlen(index->doc)+1);
							strcpy(new->doc,index->doc);
							new->next=NULL;
							newindex->next=new;
							newindex=new;
						}	
					}
					index=index->next;
				}
				freeSubChainFromMemory(querylist);
				querylist=newlist;
			}
			break;
		}
		chain=chain->next;
	}
	if (wordfound==1)
	{
		if (cachemode==-1)
		{
			cachemode=1;
		}
	}
	if (wordfound==0)
	{
		if (cachemode==-1)
		{
			cachemode=0;
		}
		freeSubChainFromMemory(querylist);
		querylist=NULL;
	}
	return querylist;
}

struct occlist *so(struct occlist *querylist,struct wordlist *chain,char *word)
{
	/*implements logical OR search*/
	struct occlist *subchain;
	struct occlist *index;
	int wordfound=0;
	while(chain!=NULL)/*iterates thought the wordlist until the word is found or the end of the list is reached*/
	{
		if (strcmp(chain->term,word)==0)
		{
			/*if the word is found.....*/
			wordfound=1;
			subchain=chain->occhead;
			if (querylist==NULL)
			{/*if the current querylist is null, the entire sublist of the term is copied to the querylist*/
				while(subchain!=NULL)
				{
					if (querylist==NULL)
					{
						querylist = (struct occlist*) malloc(sizeof(struct occlist));
						querylist->doc = (char *) malloc(strlen(subchain->doc)+1);
						strcpy(querylist->doc,subchain->doc);
						querylist->next=NULL;
						index=querylist;
					}
					else
					{
						struct occlist *new = (struct occlist *) malloc(sizeof(struct occlist));
						new->doc = (char *) malloc(strlen(subchain->doc)+1);
						strcpy(new->doc,subchain->doc);
						new->next=NULL;
						index->next=new;
						index=new;
					}
					subchain=subchain->next;
				}
			}
			else
			{
				/*if the querylist is not null, a newlist*/
				struct occlist *newlist=NULL;
				struct occlist *newindex;	
				while(subchain!=NULL)
				{
					if (belongsToSublist(subchain->doc,querylist)==0)
					{
						if (newlist==NULL)
						{
							newlist = (struct occlist*) malloc(sizeof(struct occlist));
							newlist->doc = (char *) malloc(strlen(subchain->doc)+1);
							strcpy(newlist->doc,subchain->doc);
							newlist->next=NULL;
							newindex=newlist;
						}
						else
						{
							struct occlist *new = (struct occlist *) malloc(sizeof(struct occlist));
							new->doc = (char *) malloc(strlen(subchain->doc)+1);
							strcpy(new->doc,subchain->doc);
							new->next=NULL;
							newindex->next=new;
							newindex=new;
						}	
					}
					subchain=subchain->next;
				}
				append_occ(querylist,newlist);
			}
			break;
		}
		chain=chain->next;
	}
	if (wordfound==1)
	{
		if (cachemode==-1)
		{
			cachemode=1;
		}
	}
	if (wordfound==0)
	{
		if (cachemode==-1)
		{
			cachemode=0;
		}
	}
	return querylist;	
}

int isDirectory(char* dir)
{
	/*this method returns 1 if the string specified is a directory, and 0 otherwise*/
	struct stat dir_op;
	stat(dir, &dir_op);
	return S_ISDIR(dir_op.st_mode);
}

int doesFileExist(char *filename)
{
	/*returns 1 if the file exists, and 0 otherwise*/
	FILE *f;
	int ret=0;
	if ((f = (fopen(filename, "r"))))
	{
		fclose(f);
		ret=1;
	}
	return ret;
}

void freeSubChainFromMemory(struct occlist *subchain)
{
	/*this method recurses to the end of the occurence list, and frees each link from memory on the way back*/
	if (subchain==NULL)
	{
		return;
	}
	else
	{
		freeSubChainFromMemory(subchain->next);
		free(subchain->doc);
		free(subchain);
	}
}

void freeChainFromMemory(struct wordlist *chain)
{
	/*this method recurses to the end of the wordlist, and frees each link along with its corresponding occurence list from memory on the way back*/
	if (chain==NULL)
	{
		return;
	}
	else
	{
		freeChainFromMemory(chain->next);
		freeSubChainFromMemory(chain->occhead);
		free(chain->term);
		free(chain);
	}
}

unsigned int computeCacheSize(char *cache)
{
	/*computes the cache size in bytes and returns it as an unsigned integer*/
	int i;
	int s;
	char size[100];
	for (i=0;i<strlen(cache);i++)
	{
		if ((cache[i]>=48) && (cache[i]<=57))
		{
			size[i]=cache[i];
		}
		else
		{
			size[i]='\0';
			break;
		}
	}
	s=atoi(size);
	if ((cache[i]=='K')&&(cache[i+1]=='B'))
	{
		s=s*1024;
	}
	else if ((cache[i]=='M')&&(cache[i+1]=='B'))
	{
		s=s*1048576;
	}
	else if ((cache[i]=='G')&&(cache[i+1]=='B'))
	{
		s=s*1073741824;
	}
	else
	{
		s=-1;
	}
	return s;
}

struct searchquery *addword(struct searchquery *S,char *word)
{
	/*this method adds a word to the array and increments the index*/
	S->sq[S->sq_index]=(char *) malloc(strlen(word)+1);
	strcpy(S->sq[S->sq_index],word);
	S->sq_index++;
	return S;
}

struct searchquery *sortwordarray(struct searchquery *S)
{
	/*given an array of words, this method sorts the words in lexicographical order*/
	int i;
	int j;
	int ptr;
	for (i=1;i<(S->sq_index);i++)
	{
		char *temp=NULL;
		ptr=i;
		for (j=i;j<(S->sq_index);j++)
		{
			if (strcmp(S->sq[j],S->sq[ptr])<0)
			{
				ptr=j;	
			}
		}
		temp=S->sq[i];
		S->sq[i]=S->sq[ptr];
		S->sq[ptr]=temp;
	}
	return S;
}

struct wordlist *readindex(FILE *infile,struct wordlist *chain,unsigned int cache_size)
{
	/*this method reads the index file into memory*/
	struct wordlist *wordindex=chain;
	struct occlist *occindex=NULL;
	unsigned int RESTRAINT=cache_size;
	unsigned int USED=0;
	char line[9000];
	char word[9000];
	int listindex;
	int charindex;
	int count;
	int i;
	int let;
	int listmode=0;
	listindex=0;
	charindex=0;
	count=0;
	while (fgets(line,9000,infile) != NULL)/*loop which processes each file line by line*/
	{
		let=0;
		for (i=0;i<strlen(line);i++)
		{
			if ((line[i]>=32) && (line[i]<=126))
			{
				if (let==0)
				{
					charindex=0;
					let=1;
					count++;
				}
				word[charindex]=tolower(line[i]);
				charindex++;
				word[charindex]='\0';
				if (i==strlen(line)-1)
				{
					if (strcmp("<list>",word)==0)
					{
						listmode=1;
					}
					else if (strcmp("</list>",word)==0)
					{
						listmode=0;
						if ((USED>RESTRAINT)&&(RESTRAINT>0))
						{
							/*if the cache is full, the list is returned the way it is*/
							return chain;
						}
					}
					else
					{
						if (listmode==1)
						{
							if (chain==NULL)
							{
								/*starts the wordlist if it is currently null*/
								chain = (struct wordlist *) malloc(sizeof(struct wordlist));
								chain->term = (char *) malloc(strlen(word)+1);
								strcpy(chain->term,word);
								chain->occhead=NULL;
								chain->next=NULL;
								wordindex=chain;
								cacherange=chain->term;
								USED=USED+sizeof(struct wordlist)+strlen(word)+1;
							}
							else
							{
								/*the next term is added on to the wordlist*/
								struct wordlist *new = (struct wordlist *) malloc(sizeof(struct wordlist));
								new->term = (char *) malloc(strlen(word)+1);
								strcpy(new->term,word);
								new->occhead=NULL;
								new->next=NULL;
								wordindex->next=new;
								wordindex=new;
								cacherange=new->term;
								USED=USED+sizeof(struct wordlist)+strlen(word)+1;
							}
							listmode=2;
						}
						else if (listmode==2)
						{
							if (wordindex->occhead==NULL)
							{
								
								/*starts the wordlist if it is currently null*/
								wordindex->occhead = (struct occlist *) malloc(sizeof(struct occlist));
								wordindex->occhead->doc = (char *) malloc(strlen(word)+1);
								wordindex->occhead->next=NULL;
								strcpy(wordindex->occhead->doc,word);
								occindex=wordindex->occhead;
								USED=USED+sizeof(struct occlist)+strlen(word)+1;
							}
							else
							{
								/*the next filename is added to the wordlist*/
								struct occlist *occnext= (struct occlist *) malloc(sizeof(struct occlist));
								occnext->doc = (char *) malloc(strlen(word)+1);
								occnext->next=NULL;
								strcpy(occnext->doc,word);
								occindex->next=occnext;
								occindex=occnext;
								USED=USED+sizeof(struct occlist)+strlen(word)+1;
							}
							listmode=3;
						}
						else if (listmode==3)
						{
							listmode=2;
						}
						
					}				
				}
			}
			else
			{
				
				if (let==1)
				{
					let=0;
					word[charindex]='\0';
					if (strcmp("<list>",word)==0)
					{
						listmode=1;
					}
					else if (strcmp("</list>",word)==0)
					{
						listmode=0;
						if ((USED>RESTRAINT)&&(RESTRAINT>0))
						{
							remaining=0;
							return chain;
						}
					}
					else
					{
						
						if (listmode==1)
						{
							
							if (chain==NULL)
							{
								/*starts the wordlist if it is currently null*/
								chain = (struct wordlist *) malloc(sizeof(struct wordlist));
								chain->term = (char *) malloc(strlen(word)+1);
								chain->occhead=NULL;
								strcpy(chain->term,word);
								chain->next=NULL;
								wordindex=chain;
								cacherange=chain->term;
								USED=USED+sizeof(struct wordlist)+strlen(word)+1;
							}
							else
							{
								/*the next term is added on to the wordlist*/
								struct wordlist *new = (struct wordlist *) malloc(sizeof(struct wordlist));
								new->term = (char *) malloc(strlen(word)+1);
								strcpy(new->term,word);
								new->next=NULL;
								new->occhead=NULL;
								wordindex->next=new;
								wordindex=new;
								cacherange=new->term;
								USED=USED+sizeof(struct wordlist)+strlen(word)+1;
							}
							listmode=2;
							
						}
						else if (listmode==2)
						{
							if (wordindex->occhead==NULL)
							{
						
								/*starts the wordlist if it is currently null*/
								wordindex->occhead = (struct occlist *) malloc(sizeof(struct occlist));
								wordindex->occhead->doc = (char *) malloc(strlen(word)+1);
								wordindex->occhead->next=NULL;
								strcpy(wordindex->occhead->doc,word);
								occindex=wordindex->occhead;
								USED=USED+sizeof(struct occlist)+strlen(word)+1;
								
							}
							else
							{
							
								/*the next filename is added to the wordlist*/
								struct occlist *occnext= (struct occlist *) malloc(sizeof(struct occlist));
								occnext->doc = (char *) malloc(strlen(word)+1);
								occnext->next=NULL;
								strcpy(occnext->doc,word);		
								occindex->next=occnext;
								occindex=occnext;
								USED=USED+sizeof(struct occlist)+strlen(word)+1;
							}
							
							listmode=3;
						}
						else if (listmode==3)
						{
							listmode=2;
						}
						
					}
				}
				
			}
		}
	}
	if (USED>RESTRAINT)
	{
		remaining=0;
	}
	else
	{
		remaining=RESTRAINT-USED;
	}
	return chain;
}

char *nextword(char *line)
{
	/*given a line from the file, this methood tokenizes the word and returns it*/
	int i;
	int let=0;
	int next=0;
	int charindex;
	char word[9000];
	char *ret;
	for (i=0;i<strlen(line);i++)
	{
		if ((line[i]>=32) && (line[i]<=126))
		{
			if (let==0)
			{
				charindex=0;
				let=1;
			}
			word[charindex]=tolower(line[i]);
			charindex++;
			word[charindex]='\0';
			if (i==strlen(line)-1)
			{
				if (strcmp("<list>",word)==0)
				{
					next=1;
				}
				else if (next==1)
				{
					ret=(char *) malloc(strlen(word)+1);
					strcpy(ret,word);
					return ret;
				}
			}
		}
		else
		{
			if (let==1)
			{
				let=0;
				word[charindex]='\0';
				if (strcmp("<list>",word)==0)
				{
					next=1;
				}
				else if (next==1)
				{
					ret=(char *) malloc(strlen(word)+1);
					strcpy(ret,word);
					return ret;
				}
			}
		}
	}
	return NULL;
}

unsigned int fileseek(char *filename,char *word, unsigned int cache_size)
{
	/*this method returns the character index for readindex to begin reading into the cache*/
	FILE *cachestart=NULL;
	FILE *cacheend=NULL;
	unsigned int upindex=0;
	unsigned int downindex=cache_size;
	char line[9000];
	int reps;
	char *next;
	int signal=0;
	cachestart = fopen(filename, "r");
	cacheend = fopen(filename, "r");
	fseek(cacheend,downindex,SEEK_SET);
	if (fgets(line,9000,cacheend)==NULL)
	{
		fseek(cachestart,upindex,SEEK_SET);
		return upindex;				
	}
	while(1)
	{
		reps=0;
		signal=0;
		fseek(cachestart,downindex,SEEK_SET);
		while (fgets(line,9000,cachestart) != NULL)
		{
			if (reps==1)
			{
				next=nextword(line);
				signal=1;
				break;
			}
			else
			{
				downindex=downindex+strlen(line);
			}
			reps++;
		}
		if (strcmp(word,next)<0)
		{
			fseek(cachestart,upindex,SEEK_SET);
			if (signal==1)
			{
				free(next);
			}
			return upindex;	
		}
		else
		{
			if (signal==1)
			{
				free(next);
			}
			fseek(cacheend,downindex,SEEK_SET);
			if (fgets(line,9000,cacheend)==NULL)
			{
				fseek(cachestart,upindex,SEEK_SET);
				return upindex;				
			}
			else
			{
				fseek(cachestart,downindex,SEEK_SET);
				upindex=downindex;
				downindex=downindex+cache_size;
			}
		}
	}
}

struct wordlist *cacheindex(char *word,struct wordlist *chain,char *filename, unsigned int cache_size)
{
	/*this method sets the cache to the correct range for the search*/
	FILE *infile;
	unsigned int charindex;
	if (cache_size==0)
	{
		/*if no cache size is specified, the entire index file is read into the cache*/
		if (chain==NULL)
		{
			infile = fopen(filename, "r");
			chain=readindex(infile,chain,cache_size);
			fclose(infile);
			return chain;
		}
		else
		{
			return chain;
		}
	}
	else
	{
		if (chain==NULL)
		{
			/*cache miss*/
			cachemode=0;
			charindex=fileseek(filename,word,cache_size);
			infile = fopen(filename, "r");
			fseek(infile,charindex,SEEK_SET);
			chain=readindex(infile,chain,cache_size);
			fclose(infile);
			return chain;
		}
		else
		{
			if ((strcmp(chain->term,word)<=0)&&strcmp(word,cacherange)<=0)
			{
				/*cache hit if word is found, miss otherwise*/
				return chain;
			}
			else
			{
				/*cache miss*/
				cachemode=0;
				freeChainFromMemory(chain);
				chain=NULL;
				charindex=fileseek(filename,word,cache_size);
				infile = fopen(filename, "r");
				fseek(infile,charindex,SEEK_SET);
				chain=readindex(infile,chain,cache_size);
				fclose(infile);
				return chain;	
			}
		}
		return chain;
	}
}

struct wordlist *searchfor(struct searchquery *current,char *filename,unsigned int cache_size,struct wordlist *chain)
{
	/*this method calls the search functions(so&sa) on the list of search terms in lexicographical order
	the query results are printed, and if a cache was specified then the cache details are printed*/
	int i;
	struct occlist *querylist=NULL;
	if (cache_size>0) printf("\nCACHE DETAILS:\n\n");
	cachemode=-1;
	for (i=1;i<(current->sq_index);i++)
	{
		cachemode=-1;
		chain=cacheindex(current->sq[i],chain,filename,cache_size);
		if (strcmp(current->sq[0],"sa")==0)
		{
			querylist=sa(querylist,chain,current->sq[i]);
			if (querylist==NULL)
			{
				break;
			}
		}
		else if (strcmp(current->sq[0],"so")==0)
		{
			querylist=so(querylist,chain,current->sq[i]);
		}
		if (cachemode==1)
		{
			if (cache_size>0)
			{
				printf("Cache hit for %s. ",current->sq[i]);
				printf("%d bytes remaining in cache.\n",remaining);
			}
		}
		if (cachemode==0)
		{
			if (cache_size>0)
			{
				printf("Cache miss for %s. ",current->sq[i]);
				printf("%d bytes remaining in cache.\n",remaining);
			}
		}
	}
	printf("\n\nSEARCH RESULTS:\n\n");
	if (querylist!=NULL)
	{
		prePrintpostFree_query(querylist);
		querylist=NULL;
	}
	else
	{
		printf("NONE\n");
	}
	printf("\n");
	return chain;
}

void searchCommand(char *filename,unsigned int cache_size)
{
	/*this method continuously runs the searchcommand prompt until the prompt is terminated with a 'q'*/
	char *line=(char *) malloc(9000);
	struct wordlist *chain=NULL;
	char word[9000];
	struct searchquery *current=(struct searchquery*) malloc(sizeof(struct searchquery));
	int i;
	int charindex;
	int let;
	current->sq=(char**)malloc(sizeof(char*) * 500);
	while(1)
	{	
		current->sq_index=0;
		printf("search> ");
		fgets (line,9000, stdin);
		if (line[strlen(line)-1]=='\n')
		{
			line[strlen(line)-1]='\0';
		}
		if (strcmp(line,"q")==0)
		{
			break;
		}
		else
		{
			let=0;
			for (i=0;i<strlen(line);i++)
			{
				if (((line[i]>=65) && (line[i]<=90))||((line[i]>=97)
				&& (line[i]<=122))||((line[i]>=48) && (line[i]<=57)))
				{
					if (let==0)
					{
						charindex=0;
						let=1;
					}
					word[charindex]=tolower(line[i]);
					charindex++;
					word[charindex]='\0';
					if (i==strlen(line)-1)
					{
						current=addword(current,word);
					}
				}
				else
				{
					if (let==1)
					{
						let=0;
						word[charindex]='\0';
						current=addword(current,word);
					}
				}
			}
			current=sortwordarray(current);
			chain=searchfor(current,filename,cache_size,chain);
			for (i=0;i<(current->sq_index);i++)
			{
				free(current->sq[i]);
				
			}

		}
	}
	free(current->sq);
	free(current);
	freeChainFromMemory(chain);	
	free(line);
}


void guisearch(char *filename,unsigned int cache_size,char *line)
{
	/*this method continuously runs the searchcommand prompt until the prompt is terminated with a 'q'*/
	struct wordlist *chain=NULL;
	char word[9000];
	struct searchquery *current=(struct searchquery*) malloc(sizeof(struct searchquery));
	int i;
	int charindex;
	int let;
	current->sq=(char**)malloc(sizeof(char*) * 500);
	current->sq_index=0;
	let=0;
	for (i=0;i<strlen(line);i++)
	{
		if (((line[i]>=65) && (line[i]<=90))||((line[i]>=97)
							&& (line[i]<=122))||((line[i]>=48) && (line[i]<=57)))
		{
			if (let==0)
			{
				charindex=0;
				let=1;
			}
			word[charindex]=tolower(line[i]);
			charindex++;
			word[charindex]='\0';
			if (i==strlen(line)-1)
			{
				current=addword(current,word);
			}
		}
		else
		{
			if (let==1)
			{
				let=0;
				word[charindex]='\0';
				current=addword(current,word);
			}
		}
	}
	current=sortwordarray(current);
	chain=searchfor(current,filename,cache_size,chain);
	for (i=0;i<(current->sq_index);i++)
	{
		free(current->sq[i]);
		
	}
	free(current->sq);
	free(current);
	freeChainFromMemory(chain);	
}

int main(int argc, char *argv[])
{
	guisearch("testlogic",0,argv[1]);
	return 0;
}
