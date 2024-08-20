
# Etapa 1: Criar a tabela no DynamoDB para este tutorial

Neste tutorial, você criará uma tabela do DynamoDB com esses atributos para registrar os dados dos dispositivos imaginários de sensores climáticos:

 - **sample_time** é uma chave primária e descreve a hora em que a amostra
   foi registrada.
  - **device_id** é uma chave de classificação e descreve o dispositivo que
   forneceu a amostra
  - **device_data** são os dados recebidos do dispositivo e formatados pela
   instrução de consulta de regra

Para criar uma tabela do DynamoDB para este tutorial
Abra o console DynamoDB, e selecione Criar tabela.

Em Criar tabela:

 1. Em Nome da tabela, insira o nome da tabela: wx_data.
    
   2. Em Chave de partição insira **sample_time**, e na lista de opções ao
    lado do campo, escolha Number.
    
   3. Em Chave de classificação, insira **device_id**, e, na lista de opções
    ao lado do campo, escolha Number.
    4. Na parte inferior da página, selecione Criar.

Você definirá **device_data** posteriormente, quando configurar a ação da regra do DynamoDB.

# Etapa 2: criar uma AWS IoT regra para enviar dados para a tabela do DynamoDB

Nesta etapa, você usará a consulta de regras para formatar os dados dos dispositivos imaginários de sensores climáticos para gravar na tabela do banco de dados.

Um exemplo de carga útil de mensagem recebida de um dispositivo de sensor climático tem o seguinte aspecto:
```
    {
      "temperature": 28,
      "humidity": 80,
      "barometer": 1013,
      "wind": {
        "velocity": 22,
        "bearing": 255
      }
    }
```
Para a entrada do banco de dados, você usará a instrução de consulta de regra para nivelar a estrutura da carga útil da mensagem para ficar dessa forma:
```
    {
      "temperature": 28,
      "humidity": 80,
      "barometer": 1013,
      "wind_velocity": 22,
      "wind_bearing": 255
    }
```
Nessa regra, você também usará alguns Modelos de substituição. Os modelos de substituição são expressões que permitem inserir valores dinâmicos de funções e dados de mensagens.

### Para criar a AWS IoT regra para enviar dados para a tabela do DynamoDB

 1. Abra o hub Regras do AWS IoT console. Ou você pode abrir a AWS IoT página inicial em AWS Management Console e navegar até Roteamento de mensagens > Regras.
 2. Para começar a criar sua nova regra em Regras, escolha Criar regra.
 3. Em Propriedades da regra:
 **a.** Em Nome do perfil, insira **wx_data_ddb**.
 Lembre-se de que o nome de uma regra deve ser exclusivo em sua região Conta da AWS e não pode ter espaços. Usamos um caractere sublinhado nesse nome para separar as duas palavras no nome da regra.
 **b.** Em Descrição da regra, descreva a regra.
 Uma descrição significativa facilita lembrar o que essa regra faz e por que você a criou. A descrição pode ser tão longa quanto necessário, portanto, seja o mais detalhista possível.

 4. Escolha **Próximo** para continuar.
 5. Em instrução SQL:
 a. Na versão SQL, selecione2016-03-23.
 b. Na caixa de edição da instrução SQL, insira a instrução:
```
    SELECT temperature, humidity, barometer, wind.velocity as wind_velocity, wind.bearing as wind_bearing, FROM 'device/+/data'
```
Esta declaração:

 - Recebe mensagens MQTT com um tópico que corresponda ao filtro de tópicos device/+/data.
 - Formata os elementos do atributo wind como atributos individuais.
 - Transmite o temperature, humidity e outros atributos.

 6. Escolha Próximo para continuar.
 7. Em Ações de regra:
	 a. Para abrir a lista de ações de regra para essa regra, na **Ação 1**, escolha DynamoDB.	 
     b. Em Nome da tabela, escolha o nome da tabela do DynamoDB que você criou na etapa anterior: **wx_data**. Os campos **Tipo de chave de partição** e **Tipo de chave de classificação** são preenchidos com os valores da tabela do DynamoDB.

    c. Em **Chave de partição**, insira **sample_time**.
    d. Em **Valor da chave de partição**, insira **${timestamp()}**.
    Esse é o primeiro dos [Modelos de substituição](https://docs.aws.amazon.com/pt_br/iot/latest/developerguide/iot-substitution-templates.html) que você usará nesta regra. Em vez de usar um valor da carga da mensagem, ele usará o valor retornado da função de datação.

    e. Em **Chave de classificação**, insira **device_id**.
    f. Em **Valor da chave de classificação**, insira **${cast(topic(2) AS DECIMAL)}**.
    Esse é o segundo dos Modelos de substituição que você vai usar nesta regra. Ele insere o valor do segundo elemento no nome do tópico, que é o ID do dispositivo, depois de convertê-lo em um valor DECIMAL para corresponder ao formato numérico da chave. 
    g. Na coluna **Gravar os dados da mensagem nesta coluna**, insira **device_data**. Isso criará a coluna **device_data** na tabela do DynamoDB.
    h. Deixe o campo Operação em branco.
    i. Em Perfil do IAM, selecione Criar novo perfil.
    j. Na caixa de diálogo Criar perfil, em Nome do perfil, insira wx_ddb_role. Essa nova função conterá automaticamente uma política com o prefixo "aws-iot-rule" que permitirá que a wx_data_ddb regra envie dados para a tabela do wx_data DynamoDB que você criou.
    k. Em Perfil do IAM, escolha wx_ddb_role.
    l. Na parte inferior da página, selecione a opção Próximo.

8. Na parte inferior da página Revisar e criar, escolha a opção Criar para criar a regra.

# Etapa 3: testar a AWS IoT regra e a tabela do DynamoDB

Para testar a nova regra, você usará o cliente MQTT para publicar e assinar as mensagens MQTT usadas neste teste.

Abra o cliente MQTT no AWS IoT console em uma nova janela. Isso permitirá que você edite a regra sem perder a configuração do seu cliente MQTT. O cliente MQTT não retém nenhuma assinatura ou logs de mensagens se você deixar que ele vá para outra página no console. Você também desejará que uma janela de console separada seja aberta para o hub do DynamoDB Tables no console para visualizar AWS IoT as novas entradas que sua regra envia.

### Você pode usar o cliente MQTT para testar a regra

 1. No cliente MQTT no AWS IoT console, assine o tópico **device/+/data**.
	 a. No Cliente MQTT, escolha Assinar um tópico.
	 b. Em Filtro de tópicos, insira o tópico **device/+/data**.
	 c. Escolha Assinar.
	 
 2. Agora, publique uma mensagem no tópico de entrada com um ID de dispositivo específico, **device/22/data**. Você não pode publicar nos tópicos MQTT que contenham caracteres curinga.
    a. No cliente MQTT, escolha Publicar em um tópico.
    b. Em **Nome do tópico**, insira um nome para o tópico, device/22/data.
    c. Em **Carga útil da mensagem**, insira os seguintes dados de exemplo.

```
    {
      "temperature": 28,
      "humidity": 80,
      "barometer": 1013,
      "wind": {
        "velocity": 22,
        "bearing": 255
      }
    }
```
   d. Para publicar a mensagem MQTT, escolha Publicar.
   e. Agora, no cliente MQTT, escolha Assinar um tópico. Na coluna Assinar, escolha a assinatura device/+/data. Confirme se os dados de amostra da etapa anterior aparecem.

 3. Verifique se consegue visualizar a linha na tabela do DynamoDB que sua regra criou.
	 a. No hub do DynamoDB Tables no console, escolha **wx_data** e, em AWS IoT seguida, escolha a guia Itens.
	 b. Se você já estiver na guia Itens, talvez precise atualizar a exibição selecionando o ícone de atualização no canto superior direito do cabeçalho da tabela.
	 b. Observe que os valores de sample_time na tabela são links. Abra um. Se acabou de enviar a primeira mensagem, ela será a única na lista.
	 c. Esse link exibe todos os dados nessa linha da tabela.
	 d. Expanda a entrada device_data para ver os dados que resultaram da consulta da regra.
	 e. Explore as diferentes representações dos dados disponíveis nessa exibição. Você também pode editar os dados nessa exibição.

