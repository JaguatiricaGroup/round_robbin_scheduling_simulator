import os,shutil,sys
from subprocess import call
import threading as th
import glob
# from datetime import date
# argumentos = ['3', '6', '12', '24']
# arquiteturas = ['25_10', '50_25', '100_50', '300_150']
numExecucoesPorExperimento = 4
meses = 12
# data = date.timetuple()
# print(data)

#parou na 3

#colocar os meses em paralelo e um arquivo com o treino preparado e compartilhado entre as execucoes
def executa(i):
    # print('Execucao:', i)
    # os.execv('main.py',['nada'])
    # cmd = ["python", "experimento15-12-17.py"]
    cmd = ["./trabalhoso"]

    # print(cmd)
    nada = call(cmd)
    print('nadaaa', nada)
    # testeDaRNA_ex_0mes_10.csv    

    # call(["mv", "treinoDaRNA_ex_"+str(i)+'meses_*.csv', 'experimentos/experimento_'+str(i)+'/'])

# call(["rm", "-R", "testeDaRNA.csv"])
# call(["rm", "-R", "treinoDaRNA.csv"])
call(["rm", "-R", "experimentos"])
os.mkdir('experimentos')
threads = list()
for i in range(numExecucoesPorExperimento):
    # os.mkdir('experimentos/experimento_'+str(i))
    threads.append(th.Thread(target=executa, args=(i,)))
    threads[i].start()

# executa
for i in range(numExecucoesPorExperimento):
    threads[i].join()

print('terminei')
    # shutil.copy('experimento','experimentos/'+ 'execucao_' +str(i))

#executa os testes no numeros de vezes requerido
# for i in range(numExecucoesPorExperimento):
#     print(argumentos[0], arquiteturas[0])
#     # os.execv('main.py', [argumentos[1],arquiteturas[1]])
#     os.exec('main.py')
#     os.mkdir('experimentos')
#     shutil.copy('experimento','experimentos/'+
#                 arquiteturas[0]+'_'+argumentos[0]+'_'+str(i))
