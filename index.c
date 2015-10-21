#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include "index.h"

/*index.c designed by Eric J D'Souza
Given a directory, and index file is created for search
*/

struct occlist /*linked list of filenames with their corresponding occurences*/
{
	char *doc;
	int occ;
  	struct occlist *next;
};

struct wordlist /*linked list of words; each word also has an occlist which branches off which contains the occurences for each file*/
{
	char *term;
	struct wordlist *next;
	struct occlist *occhead;
};

void printlist (struct wordlist *p)
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
		        	printf("%s %d ",inside->doc,inside->occ);
				inside=inside->next;
		    	} 
                	temp=temp->next;
			printf("\n");
            	} 
      	}
      	else
        printf("THE INDEX IS EMPTY\n");
}

struct wordlist *indexword(char *word,struct wordlist *chain,char *filename)
{
	/*given a word, and the filename it was found in, the word and the occurence is added to the list, preserving lexicographical order*/
	struct wordlist *iterator=chain;
	if (chain==NULL)/*if the list is null, the word and the occurence becomes the first element of the list*/
	{
		struct wordlist *wordhead;
		struct occlist *head;
		wordhead = (struct wordlist *) malloc(sizeof(struct wordlist));
		wordhead->term = (char *) malloc(strlen(word)+1);
		strcpy(wordhead->term,word);
		wordhead->next=NULL;
		head = (struct occlist *) malloc(sizeof(struct occlist));
		head->doc=(char *) malloc(strlen(filename)+1);
		strcpy(head->doc,filename);
		head->occ=1;
		head->next=NULL;
		wordhead->occhead=head;	
		chain=wordhead;
		
	}
	else/*if the list is not null...*/
	{
		if (strcmp(word,iterator->term)<0)
		{
			/*set word as source if the current word comes first in lexicographical order*/
			struct wordlist *wordhead;
			struct occlist *head;
			wordhead = (struct wordlist *) malloc(sizeof(struct wordlist));
			wordhead->term = (char *) malloc(strlen(word)+1);
			strcpy(wordhead->term,word);
			wordhead->next=chain;
			head = (struct occlist *) malloc(sizeof(struct occlist));
			head->doc=(char *) malloc(strlen(filename)+1);
			strcpy(head->doc,filename);
			head->occ=1;
			head->next=NULL;
			wordhead->occhead=head;	
			chain=wordhead;
		}
		else/*if the word does not come first in lexicographical order*/
		{
			while(iterator!= NULL)
			{
				if (strcmp(iterator->term,word)<0)/*if word does not already exist in the list*/
				{
					if (iterator->next==NULL)
					{
						/*insert word at end of wordlist*/
						struct wordlist *new;
						struct occlist *head;
						new = (struct wordlist *) malloc(sizeof(struct wordlist));
						new->term = (char *) malloc(strlen(word)+1);
						strcpy(new->term,word);
						new->next=NULL;
						iterator->next=new;
						head = (struct occlist *) malloc(sizeof(struct occlist));
						head->doc=(char *) malloc(strlen(filename)+1);
						strcpy(head->doc,filename);
						head->occ=1;
						head->next=NULL;
						new->occhead=head;
						break;
					}
					else if (strcmp(word,iterator->next->term)<0)
					{
						/*insert word as intermediate element*/
						struct wordlist *new;
						struct occlist *head;
						new = (struct wordlist *) malloc(sizeof(struct wordlist));
						new->term = (char *) malloc(strlen(word)+1);
						strcpy(new->term,word);
						new->next=iterator->next;
						iterator->next=new;
						head = (struct occlist *) malloc(sizeof(struct occlist));
						head->doc=(char *) malloc(strlen(filename)+1);
						strcpy(head->doc,filename);
						head->occ=1;
						head->next=NULL;
						new->occhead=head;	
						break;
					}
				}
				else if (strcmp(iterator->term,word)==0)/*if the word already exists in the list, the occurence just needs to be updates*/
				{
					struct occlist *subchain=iterator->occhead;
					struct occlist *ptr=iterator->occhead;
					while(ptr!=NULL)
					{
						if (strcmp(ptr->doc,filename)==0)
						{
							/*increment if subchain head matches the filename, the occurence count is simply just incremented*/
							ptr->occ=ptr->occ+1;
							break;
						}
						else if(ptr->next==NULL)
						{
							/*if filename does not exist in the list, the occurence is added to the end of the list*/
							struct occlist *new;
							new = (struct occlist *) malloc(sizeof(struct occlist));
							new->doc = (char *) malloc(strlen(filename)+1);
							strcpy(new->doc,filename);
							new->next=NULL;
							new->occ=1;
							ptr->next=new;
							break;					
						}
						if (strcmp(ptr->next->doc,filename)==0)
						{
							/*if an intermediate subchain element matches the filename, the occurences are incremented*/
							struct occlist *relink=ptr->next;
							ptr->next=ptr->next->next;
							relink->occ++;
							ptr=subchain;
							while(ptr!=NULL)/*the list is reiterated to place the updated link in a fashion to preserve decending numerical order*/
							{
								if ((ptr->occ)<(relink->occ))
								{
									relink->next=ptr;
									iterator->occhead=relink;
									break;
								}
								else if(ptr->next==NULL)
								{
									ptr->next=relink;
									relink->next=NULL;
								}
								else if((ptr->next->occ)<=(relink->occ))
								{
									relink->next=ptr->next;
									ptr->next=relink;
									break;
								}
								ptr=ptr->next;
							}
							break;
						}
						ptr=ptr->next;
					}
					
				}
				iterator=iterator->next;
			}
		}
	}
	return chain;
}

struct wordlist *indexfile(char *path,char *filename,struct wordlist *chain)
{
	/*each file is processes line by line, and each word is added to the list one by one*/
	char line[9000];
	FILE *infile;
	char word[9000];
	int listindex;
	int charindex;
	int count;
	int i;
	int let;
	infile = fopen(path, "r");
	listindex=0;
	charindex=0;
	count=0;
	while (fgets(line,9000,infile) != NULL)/*loop which processes each file line by line*/
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
					count++;
				}
				word[charindex]=tolower(line[i]);
				charindex++;
				word[charindex]='\0';
				if (i==strlen(line)-1)
				{
					chain=indexword(word,chain,filename);					
				}
			}
			else
			{
				if (let==1)
				{
					let=0;
					word[charindex]='\0';
					chain=indexword(word,chain,filename);
				}
			}
		}
	}
	return chain;
}

struct wordlist *recurseDir(char *dn,struct wordlist *c)
{
	/*if a directory name is input,this method adds each file in the directory, and recurses on each subdirectory */
	DIR* dir;
	char path[9000];
	char sub1[9000];
	char sub2[9000];
	struct wordlist *chain=c;
	struct dirent *de;
	int i;
	int position;
	for (position=0;position<strlen(dn);position++)/*builds the path name*/
	{
		path[position]=dn[position];
	}
	path[position]='/';
	position++;
	path[position]='\0';
	for (i=0;i<strlen(path);i++)
	{
		sub1[i]=path[i];
		sub2[i]=path[i];
	}
	sub1[i]='.';
	sub1[i+1]='\0';
	sub2[i]='.';
	sub2[i+1]='.';
	sub2[i+2]='\0';
	dir = opendir(dn);
	while ((de = readdir(dir)) != NULL)
	{
		char *tmp=de->d_name;
		for (i=0;i<strlen(tmp);i++)
		{
			path[i+position]=tmp[i];			
		}
		path[i+position]='\0';
		if ((isDirectory(path))&&(strcmp(path,sub1)!=0)&&(strcmp(path,sub2)!=0))/*if the path name is a directory, the method recurses on the directory*/
		{
			chain=recurseDir(path,chain);
		}
		else
		{
			if ((doesFileExist(path))&&(strcmp(path,sub1)!=0)&&(strcmp(path,sub2)!=0))/*if the path name is a file, the file is simply added*/
			{
				chain=indexfile(path,path,chain);
			}
		}		
	}
	closedir(dir);
	return chain;
}

void outputIndexToFile(char *outputFileName,char *inputFileName)
{
	FILE *outfile;
	struct wordlist *chain;
     	struct wordlist *iterator;
	struct occlist *inside;
	outfile = fopen(outputFileName,"w");
	chain=NULL;
	if (isDirectory(inputFileName))
	{
		chain=recurseDir(inputFileName,chain);
	}
	else
	{
		if (doesFileExist(inputFileName))
		{
			chain=indexfile(inputFileName,inputFileName,chain);
		}
		else
		{
			printf("ERROR: FILE OR DIRECTORY DOES NOT EXIST\n");
			return;
		}
	}
	iterator=chain;
	if(chain!= NULL)
	{
		while (iterator!= NULL)
		{
			int count=0;
			fprintf(outfile,"<list>€%s€",iterator->term);
			inside=iterator->occhead;
	       		while (inside!= NULL)
			{
				count++;
		        	fprintf(outfile,"%s€%d€",inside->doc,inside->occ);
				inside=inside->next;
				if (count==5)
				{
					break;
				}
		    	} 
                	iterator=iterator->next;
			fprintf(outfile,"</list>\n");
            	} 
      	}
	fclose(outfile);
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
		free(chain);
	}
}

int main(int argc, char *argv[])
{
	struct wordlist *chain;
	chain=NULL;
	if (argc==2)
	{
		if (isDirectory(argv[1]))
		{
			chain=recurseDir(argv[1],chain);
			printlist(chain);
			freeChainFromMemory(chain);
		}
		else
		{
			if (doesFileExist(argv[1]))
			{
				chain=indexfile(argv[1],argv[1],chain);
				printlist(chain);
				freeChainFromMemory(chain);
			}
			else
			{
				printf("ERROR: FILE OR DIRECTORY DOES NOT EXIST\n");
			}
		}
	}
	else if (argc==3)
	{
		if (isDirectory(argv[1]))
		{
			printf("ERROR: A DIRECTORY OF THE NAME '%s' ALREADY EXISTS AND CANNOT BE REPLACED\n",argv[1]);
		}
		else
		{
			if (strcmp(argv[1],"index.c")==0)
			{
				printf("ERROR: OUTPUT FILE SPECIFIED IS IN USE\n");
			}
			else
			{
			outputIndexToFile(argv[1],argv[2]);
			}
		}
	}
	else
	{
		printf("Usage:\n");
		printf("./index <inverted-index file name> <directory or file name>\n");
		printf("-OR-\n");
		printf("./index <directory or file name>\n");
	}
	return 0;
}
