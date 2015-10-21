

#This method will print the sum of all primes up to the integer n provided
def primeSum(n):
    #We initially set the sum of primes to 0, and initialize an empty list to store the primes as we find them
    sum=0;
    primelist=[]
    
    #We iterate from 2 up to n and add each prime number found to the sum
    for i in range(2,n):
        prime=True
        # PRIME TEST 1: all primes greater then 3 either take the form 6k+1 or 6k-1, when k is an integer... we can eliminate quite a few numbers here
        if (i>3 and i%6!=5 and i%6!=1):
            prime=False
        else:
            #PRIME TEST 2: we iterate through the primes we have stored which are less then i.. if i is divisble by a previously found prime, then i is not prime
            for p in primelist:
                if ((p*p)>i):
                    #we only need to check if i is divisble by primes up to the square root of i, so if we reached this point we know that i is prime and we break the loop
                    break
                
                if (i % p is 0):
                    #if i is divisible by a previously found prime that we found, we know that i is not prime and we break the loop
                    prime=False
                    break
        if (prime):
            primelist.append(i)
            sum+=i
    print sum

primeSum(100000)
