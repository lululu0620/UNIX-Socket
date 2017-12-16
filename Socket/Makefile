all:
		gcc -o client client.c
		gcc -o aws aws.c
		gcc -o serverA serverA.c
		gcc -o serverB serverB.c
		gcc -o serverC serverC.c

.PHONY: serverA serverB serverC aws

serverA: 
		./serverA

serverB:
		./serverB

serverC:
		./serverC

aws:
		./aws