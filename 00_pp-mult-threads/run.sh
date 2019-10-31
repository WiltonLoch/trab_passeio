#!/bin/bash

echo "500 750 1000 1250 1500 1750 2000" > peso-arq;
#echo "100 250" > peso-arq;
echo "1 2 3 4 6 8 10 12 16" > nroproc-arq;

for peso in `cat peso-arq` ;
  do
  for j in `cat nroproc-arq` ;
    do 
    echo -n Executando $j $peso;
    echo -n " ";
    `rm -f tmp-dados`;
    for i in  `seq 1 10` ; 
       do 
         pthreads_matrix_mult/pthreads_matrix_mult $j $peso >> tmp-dados ; 
         echo -n .;
       done ;
   echo -n " ";
   awk '{ soma += $2 
               }
             END { print "Media: ", soma/NR} ' tmp-dados;
  done
done
`rm -f peso-arq tmp-dados nroproc-arq`;

#echo "ATENCAO aos parametros:";
#echo " - tamanhos da matriz: "`cat peso-arq`
#echo " - nro de threads: "`cat nroproc-arq`
#echo "***********"
#echo "Proximos passos:"
#echo " - Altere (conforme necessario) o arquivo stat.sh"
#echo " - Execute: ./stat.sh"
