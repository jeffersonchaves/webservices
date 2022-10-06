
## Criar instância

**Permissão para {file}.pem key file:**
chmod 400 {file}.pem

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

## Instalação do MySql

O  [MySQL](https://www.mysql.com/)  é um sistema de gerenciamento de banco de dados de código aberto. Ele implementa o modelo relacional e utiliza a linguagem SQL (Structured Query Language) para gerenciar seus dados.

Este tutorial abordará como instalar a versão 8 do MySQL em um servidor Ubuntu. Ao completá-lo, você terá um banco de dados relacional funcionando que pode ser utilizado para desenvolver seu próximo site ou aplicativo.

### Passo 1 — Instalando o MySQL

No Ubuntu 20.04, é possível instalar o MySQL usando o repositório de pacotes APT. No momento em que este artigo foi escrito, a versão do MySQL disponível no repositório padrão do Ubuntu era a versão  8.0.19.

Para instalar o MySQL, atualize o índice de pacotes em seu servidor se ainda não tiver feito isso:

```
sudo apt update
```

Depois disso, instale o pacote  `mysql-server`:

```
sudo apt install mysql-server
```

Isso instalará o MySQL, mas não solicitará que você defina uma senha ou que faça outras alterações de configuração. Como isso deixa sua instalação do MySQL não segura, abordaremos isso a seguir.

### Passo 2 — Configurando o MySQL

Se quiser novas instalações do MySQL, execute o script de segurança incluído do DBMS. Esse script modifica algumas das opções padrão menos seguras referentes, por exemplo, a logins root remotos e usuários de exemplo.

Execute o script de segurança com o  `sudo`:

```
sudo mysql_secure_installation
```
Isso levará você através de uma série de prompts onde é possível fazer algumas alterações nas opções de segurança de sua instalação do MySQL. O primeiro prompt perguntará se você gostaria de definir o plug-in de validar senha, que pode ser usado para testar a força de sua senha do MySQL.

Caso você escolha configurar o plug-in de validar senha, o script solicitará que você escolha um nível de validação de senha. O nível mais forte — que você seleciona ao digitar  `2`  — exigirá que sua senha tenha pelo menos oito caracteres de tamanho e inclua uma mistura de caracteres em maiúsculo, minúsculo, numérico e especial.

A partir daí, pressione  `Y`  e, depois,  `ENTER`  para aceitar as configurações padrão para todas as perguntas subsequentes. Isso removerá alguns usuários anônimos e o banco de dados de teste, desativará os logins remotos para a raiz e carregará essas novas regras para que o MySQL respeite imediatamente as alterações que você fez.

Note que, embora tenha definido uma senha para o usuário  **root**  do MySQL, este usuário não está configurado para autenticar-se com uma senha ao conectar-se ao shell do MySQL. Se quiser, é possível ajustar esta configuração seguindo o Passo 3.

### Passo 3 — Ajustando a autenticação e os privilégios do usuário

Nos sistemas Ubuntu que executam o MySQL 5.7 (e as versões posteriores), por padrão, o usuário  **root**  do MySQL é configurado para autenticar usando o plug-in  `auth_socket`  e não com uma senha. Isso permite maior segurança e a usabilidade na maioria dos casos, mas também pode complicar as coisas quando for necessário permitir que um programa externo (por exemplo, o phpMyAdmin) acesse o usuário.

Para usar uma senha para conectar-se ao MySQL como  **root**, será necessário mudar seu método de autenticação de  `auth_socket`  para outro plug-in, como o  `caching_sha2_password`  ou o  `mysql_native_password`. Para fazer isso, abra o prompt do MySQL do seu terminal:

```
sudo mysql
```

Em seguida, verifique quais os métodos de autenticação cada conta de usuário do seu MySQL utilizam com o seguinte comando:

```
SELECT user,authentication_string,plugin,host FROM mysql.user;
```

```
Output
+------------------+------------------------------------------------------------------------+-----------------------+-----------+
| user             | authentication_string                                                  | plugin                | host      |
+------------------+------------------------------------------------------------------------+-----------------------+-----------+
| debian-sys-maint | $A$005$lS|M#3K #XslZ.xXUq.crEqTjMvhgOIX7B/zki5DeLA3JB9nh0KwENtwQ4 | caching_sha2_password | localhost |
| mysql.infoschema | $A$005$THISISACOMBINATIONOFINVALIDSALTANDPASSWORDTHATMUSTNEVERBRBEUSED | caching_sha2_password | localhost |
| mysql.session    | $A$005$THISISACOMBINATIONOFINVALIDSALTANDPASSWORDTHATMUSTNEVERBRBEUSED | caching_sha2_password | localhost |
| mysql.sys        | $A$005$THISISACOMBINATIONOFINVALIDSALTANDPASSWORDTHATMUSTNEVERBRBEUSED | caching_sha2_password | localhost |
| root             |                                                                        | auth_socket           | localhost |
+------------------+------------------------------------------------------------------------+-----------------------+-----------+
5 rows in set (0.00 sec)
```

Neste exemplo, é possível ver que o usuário  **root**  autenticou usando o plug-in  `auth_socket.`  Para configurar a conta  **root**  para autenticar-se com uma senha, execute uma instrução  `ALTER USER`  para alterar qual plug-in de autenticação ela utiliza e configure uma nova senha.

Certifique-se de alterar a  `password`  para uma senha forte de sua escolha. **Saiba que este comando mudará a senha  **root**  que você definiu no Passo 2:**

```
ALTER USER 'root'@'localhost' IDENTIFIED WITH caching_sha2_password BY 'password';
```

Então, execute  `o comando FLUSH o qual`  diz para o servidor recarregar as tabelas de permissões e colocar as suas alterações em vigor:

```
FLUSH PRIVILEGES;
exit;
```
**Nota: se tiver a autenticação por senha habilitada para root (como descrito nos parágrafos anteriores), será necessário usar um comando diferente para acessar o shell do MySQL. A sintaxe abaixo executará o seu cliente MySQL com privilégios regulares de usuário. Você somente terá privilégios de administrador dentro do banco de dados através desta autenticação:**

```
mysql -u root -p
```

### Passo 4 — Testando o MySQL

Independentemente de como você o instalou, o MySQL já deve ter sido inicializado automaticamente. Para testar isso, verifique o status dele.

```
systemctl status mysql.service
```

## Implantando a Aplicação

### Implantando a Aplicação

O último passo é realizar o `deploy` da aplicação no Tomcat recém instalado. 

Para tanto, é necessário realizar o **build** da aplicação e enviar a copia do arquivo gerado para o servidor **aws**.

```
scp -i "IFPR Iot.pem" ./../app.war ubuntu@ec2-18-228-85-221.sa-east-1.compute.amazonaws.com:/opt/tomcat/webapps
``` 

### Configurando o Banco de Dados

```
https://github.com/jeffersonchaves/webservices/blob/main/database.sql
```
