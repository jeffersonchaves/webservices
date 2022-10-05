Notas de aula, gentilmente cedidas pelo colega Thiago:

Criar instância:

1. Instances (left menu) > Instances > Launch instances
2. Name = IoTAPI (por exemplo);
3. Application and OS Images > selecionar imagem (AMI) = Ubuntu LTS;
4. Instance type = t2.micro ;
5. Key pair (login) > Create new key pair > Key pair name = AWS_EC2_key-pair (por exemplo) > Private key file format = .pem > Create key pair. Será feito download do arquivo AWS_EC2_key-pair.pem , que serah utilizado para fazer conexão segura com a VM.
6. Network settings > Allow SSH traffic from anywhere (0.0.0.0/0); Allow HTTPs traffic from the internet = ON; Allow HTTP traffic from the internet = ON;
7. Launch instance > View all instances > reload page (F5). 

Conectar-se remotamente à instância:

1. Clicar na instance ID para exibir informacoes sobre a VM recem criada.
Conect button (top right) > SSH client > copiar a string de conexão, por exemplo:
  $ ssh -i "AWS_EC2_key-pair.pem" ubuntu@ec2...compute-1.amazonaws.com
2. Acessá-la remotamente: através do computador local:
  1. Terminal > entrar no caminho onde está o arquivo .pem e corrigir a permissão (apenas leitura):
    $ chmod 400 *.pem
  2. Colar a string de conexao (SSH).
3. Testar ping (https://stackoverflow.com/questions/21981796/cannot-ping-aws-ec2-instance):
  1. EC2 > Network & Security > Security Groups > selecionar o grupo (launch-wizard-1, naum default) > Inbound rules > Edit inbound rules > Add rule > Custom ICMP; Protocol = Echo Request; Source = Anywhere-IPv4 (0.0.0.0/0); Description = ICMP_ping > Save rules.
  2. Do computador remoto: $ ping ec2...compute-1.amazonaws.com
