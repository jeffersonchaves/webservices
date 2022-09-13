## Criar instância

**Permissão para {file}.pem key file:**
chmod 600 {file}.pem

**Testar conexão ssh**
ssh -i "Ifpr-IoT-api-keys.pem" ubuntu@ec2-3-89-161-167.compute-1.amazonaws.com


**Servidor configurado!** As próximas etapas devem realizar as instalações e configurações necessárias para executar aplicações web usando Java.

## instalar o Apache Tomcat 9

### Passo 1 — Instalar o Java

O Tomcat exige que o Java esteja instalado no servidor para que qualquer código de aplicativo Web do Java possa ser executado. Podemos atender esse requisito instalando o OpenJDK com o apt.

Primeiramente, atualize seu índice de pacotes da ferramenta apt:

```
sudo apt update
```

Então, instale o pacote Java Development Kit com a apt:

```
sudo apt install default-jdk
```
Agora que o Java está instalado, podemos criar um usuário  `tomcat`, que será usado para executar o serviço do Tomcat.

### Passo 2 — Criar o usuário do Tomcat

Para fins de segurança, o Tomcat deve ser executado como um usuário sem privilégios (ou seja, não raiz). Criaremos um usuário e um grupo que executarão o serviço Tomcat.

Primeiro, crie um grupo  `tomcat`:

```
sudo groupadd tomcat
```
Em seguida, crie um usuário  `tomcat`. Tornaremos esse usuário um membro do grupo  `tomcat,`  com um diretório home de  `/opt/tomcat`  (onde vamos instalar o Tomcat) e com um shell de /`bin/false`  (para que ninguém possa fazer login na conta):

```
sudo useradd -s /bin/false -g tomcat -d /opt/tomcat tomcat
```

Agora que nosso usuário  `tomcat`  está configurado, vamos baixar e instalar o Tomcat.

### Passo 3 — Instalar o Tomcat

A melhor maneira de instalar o Tomcat 9 é baixando a última versão binária e, depois, configurá-la manualmente.

Encontre a versão mais recente do Tomcat 9 na  [página de downloads do Tomcat 9](https://tomcat.apache.org/download-90.cgi). No momento em que este artigo foi escrito, a versão mais recente disponível era a  **9.0.10**. Porém, você deverá usar uma versão posterior, se estiver disponível. Na seção  **Binary Distributions**, na lista  **Core**, copie o link para o “tar.gz”. Na maioria dos navegadores, é possível fazer isso clicando com o botão direito no link e selecionando  **Copy Link Address**, ou uma opção semelhante.

Em seguida, mude para o diretório  `/tmp`, em seu servidor. Este é um bom diretório para baixar itens transitórios, como o Tomcat tarball, dos quais não iremos precisar depois que extrairmos o conteúdo do Tomcat:

```
cd /tmp
```

Use o  `curl`  para baixar o link que copiou do site do Tomcat:

```
curl -O paste_the_copied_link_here

curl -O https://dlcdn.apache.org/tomcat/tomcat-9/v9.0.65/bin/apache-tomcat-9.0.65.tar.gz
```

Vamos instalar o Tomcat no diretório  `/opt/tomcat`. Crie o diretório e, em seguida, extraia o arquivo nele com esses comandos:

```
sudo mkdir /opt/tomcat
sudo tar xzvf apache-tomcat-*tar.gz -C /opt/tomcat --strip-components=1
```

Na sequência, podemos configurar as permissões de usuário apropriadas para a nossa instalação.

### Passo 4 — Atualizar permissões

O usuário  `tomcat`  que configuramos precisa ter acesso à instalação do Tomcat. Agora, vamos configurar isso.

Vá até o diretório onde extraímos a instalação do Tomcat:
```
cd /opt/tomcat
```

Dê a propriedade de todo o diretório de instalação ao grupo  `tomcat`:

```
sudo chgrp -R tomcat /opt/tomcat
```

Em seguida, dê ao grupo  `tomcat`  acesso de leitura ao diretório  `conf`  e todo o seu conteúdo e  **execute**  o acesso ao diretório, propriamente dito:

```
sudo chmod -R g+r conf
sudo chmod g+x conf
```

Faça do usuário  `tomcat`  o proprietário dos diretórios  `webapps`,  `work`,  `temp`  e  `logs`:

```
sudo chown -R tomcat webapps/ work/ temp/ logs/
```

Agora que as permissões apropriadas estão configuradas, podemos criar um arquivo de serviço systemd para gerenciar o processo do Tomcat.

### Passo 5 — Criar um arquivo de serviço systemd

Queremos executar o Tomcat como um serviço. Dessa forma, vamos configurar o arquivo de serviço systemd.

O Tomcat precisa saber onde o Java está instalado. Normalmente, esse caminho é chamado de **“JAVA_HOME”.** A maneira mais fácil de procurar esse local é executando este comando:

```
sudo update-java-alternatives -l
```
```
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64/bin/java
```
```
echo $JAVA_HOME
```

**Importante: Seu  `JAVA_HOME`  pode ser diferente.**

Com essa informação, podemos criar o arquivo de serviço systemd. Abra um arquivo chamado  `tomcat.service`  no diretório  `/etc/systemd/system`, digitando:

```
sudo nano /etc/systemd/system/tomcat.service
```

Cole o conteúdo a seguir no seu arquivo de serviço. Modifique o valor de  `JAVA_HOME`  se necessário para que corresponda ao valor que você encontrou no seu sistema. Você também pode desejar modificar as configurações de alocação de memória que estão especificadas em  `CATALINA_OPTS`:

```
[Unit]
Description=Apache Tomcat Web Application Container
After=network.target

[Service]
Type=forking

Environment=JAVA_HOME=/usr/lib/jvm/java-1.11.0-openjdk-amd64
Environment=CATALINA_PID=/opt/tomcat/temp/tomcat.pid
Environment=CATALINA_HOME=/opt/tomcat
Environment=CATALINA_BASE=/opt/tomcat
Environment='CATALINA_OPTS=-Xms512M -Xmx1024M -server -XX:+UseParallelGC'
Environment='JAVA_OPTS=-Djava.awt.headless=true -Djava.security.egd=file:/dev/./urandom'

ExecStart=/opt/tomcat/bin/startup.sh
ExecStop=/opt/tomcat/bin/shutdown.sh

User=tomcat
Group=tomcat
UMask=0007
RestartSec=10
Restart=always

[Install]
WantedBy=multi-user.target
```

Quando você terminar, salve e feche o arquivo.

Em seguida, recarregue o **daemon systemd** para que ele saiba sobre nosso arquivo de serviço:

```
sudo systemctl daemon-reload
```

Inicie o serviço Tomcat digitando:

```
sudo systemctl start tomcat
```

Verifique novamente se ele foi iniciado sem erros, digitando:

```
sudo systemctl status tomcat
```

### Passo 6 — Ajustar o firewall e testar o servidor do Tomcat

Agora que o serviço Tomcat foi iniciado, podemos testá-lo para garantir que a página padrão esteja disponível.

Antes de fazer isso, precisamos ajustar o firewall para permitir que nossos pedidos cheguem ao serviço. Se seguiu os pré-requisitos, você terá um firewall  `ufw`  habilitado.

O Tomcat usa a porta  `8080`  para aceitar pedidos convencionais. Permita o tráfego naquela porta, digitando:

```
sudo ufw allow 8080
```

Com o firewall modificado, é possível acessar a página inicial padrão, indo ao seu domínio ou endereço IP, seguido de  `:8080`  em um navegador Web:

```
Open in web browser
http://server_domain_or_IP:8080
```

Você verá a página inicial padrão do Tomcat, além de outras informações. No entanto, caso clique nos links para o Manager App, por exemplo, seu acesso será negado. Podemos configurar esse acesso a seguir.

Se você conseguiu acessar o Tomcat com sucesso, agora é um bom momento para habilitar o arquivo de serviço, de modo a fazer o Tomcat iniciar automaticamente na inicialização:

```
sudo systemctl enable tomcat
```
