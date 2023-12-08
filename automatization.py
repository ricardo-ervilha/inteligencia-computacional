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
prefixos_desejados = ["SET1"]

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


print(f"Numero de instancias: ", len(caminhos_ophs))

parametros = []
for caminho in caminhos_ophs:
    parametros.append([caminho])

# parametros = [["SET1 1-2/64-45-1-2"]]
for parametro in parametros:
    comando = ["./main_automatic"]+parametro
    
    for i in range(5):# rodar esse numero de vezes para cada instancia
        proc = subprocess.Popen(comando)
        proc.wait()# só passar para a próxima iteração quando terminar esse processo
        time.sleep(1)# aguardar um tempo entre cada execução para não usar sempre a mesma semente

