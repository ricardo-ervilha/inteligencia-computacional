import subprocess
import os
import time

os.chdir('./build')
proc = subprocess.Popen(["cmake",".."])
proc.wait()

proc = subprocess.Popen(["make"])
proc.wait()

# Lista de parâmetros a serem passados para o programa
pasta_instancias = "../test_instances/"
# prefixos_desejados = ["SET1", "SET2", "SET3", "SET4", "SET5"]
prefixos_desejados = ["SET1 1-2"]

def obter_caminhos_arquivos(diretorio, extensao=".ophs"):
    caminhos_arquivos = []
    for pasta_raiz, sub_pastas, arquivos in os.walk(diretorio):
        for arquivo in arquivos:
            if arquivo.endswith(extensao):
                caminhos_arquivos.append(os.path.join(pasta_raiz, arquivo))
    return caminhos_arquivos

def obter_caminhos_arquivos_em_prefixos(diretorio, prefixos, extensao=".ophs"):
    caminhos_arquivos = []
    for pasta_raiz, sub_pastas, arquivos in os.walk(diretorio):
        for sub_pasta in sub_pastas:
            if any(sub_pasta.startswith(prefix) for prefix in prefixos):
                pasta_completa = os.path.join(pasta_raiz, sub_pasta)
                for pasta, _, arquivos_pasta in os.walk(pasta_completa):
                    for arquivo in arquivos_pasta:
                        if arquivo.endswith(extensao):
                            caminhos_arquivos.append(os.path.join(pasta, arquivo))
    return caminhos_arquivos

caminhos_ophs = obter_caminhos_arquivos_em_prefixos(pasta_instancias, prefixos_desejados)

# Remover a parte "test_instances" de todos os caminhos
caminhos_ophs = [caminho.replace("../test_instances/", "") for caminho in caminhos_ophs]
caminhos_ophs = [caminho.replace(".ophs", "") for caminho in caminhos_ophs]
caminhos_ophs = sorted(caminhos_ophs)

parametros = []
for caminho in caminhos_ophs:
    parametros.append([caminho])


tamPopInit = "2"
percentualCrossOver = "0"
percentualMutacao = "0"
gama = "0"
numGenerations = "1"

parametros = [

    ["SET1 1-2/64-45-1-2", "4", "0.5", "0.8", "1.", "8"],
    ["SET1 1-2/100-35-1-2", "1", "0", "0", "1.", "1."],
    
    ["SET1 2-3/66-125-2-3", "4", "0.4", "0.7", "0.95", "5"],
    ["SET1 2-3/102-60-2-3", "6", "0.5", "0.7", "1.", "10"],
    
    ["SET1 3-4/64-70-3-4", "6", "0.5", "0.7", "0.9", "6"],
    ["SET1 3-4/100-40-3-4", "1", "0.0", "0.", "1.", "1"],
    
    ["SET2 5-3/66-60-5-3", "6", "0.6", "0.7", "0.95", "6"],
    ["SET2 5-3/100-45-5-3", "3", "0.6", "0.7", "0.95", "3"],
    
    ["SET2 6-4/66-50-6-4", "8", "0.8", "0.7", "0.95", "8"],
    ["SET2 6-4/100-35-6-4", "1", "0.0", "0.0", "0.0", "1"],
    
    ["SET3 10-4/66-125-10-4", "6", "0.8", "0.7", "0.8", "5"],
    ["SET3 10-4/100-200-10-4", "5", "0.8", "0.7", "0.8", "5"],
    
    ["SET3 12-5/64-75-12-5", "10", "0.8", "0.4", "0.8", "6"],
    ["SET3 12-5/100-180-12-5", "9", "0.8", "0.5", "0.8", "5"],
    
    ["SET4/100-20-3-3", "7", "0.8", "0.5", "0.8", "5"],
    ["SET4/100-25-3-3", "7", "0.8", "0.5", "0.8", "8"],
    ["SET4/102-35-3-3", "5", "0.8", "0.5", "0.9", "8"],
    ["SET4/102-40-3-3", "5", "0.8", "0.5", "0.9", "8"],
    ["SET4/102-45-3-3", "5", "0.8", "0.5", "0.9", "8"],

]

for parametro in parametros:
    
    comando = ["./main_automatic"]+parametro

    for i in range(10):# rodar esse numero de vezes para cada instancia
        proc = subprocess.Popen(comando)
        proc.wait()# só passar para a próxima iteração quando terminar esse processo
        
        
        os.chdir('../')
        comando_visualize = ["python3", "visualize.py", parametro[0], "1", "GA"] # plotar solução do GA
        subprocess.Popen(comando_visualize).wait()
        
        comando_visualize = ["python3", "visualize.py", parametro[0], "1", "SA"] # plotar solução do SA
        subprocess.Popen(comando_visualize).wait()
        
        os.chdir('./build')
        
        time.sleep(1)# aguardar um tempo entre cada execução para não usar sempre a mesma semente

