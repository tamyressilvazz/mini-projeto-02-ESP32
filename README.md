# Atividade de Avaliação – Revisando o Sistema de Alarme

## 1. Objetivo Geral

Desenvolver um **sistema de alarme distribuído**, utilizando **ESP32** e um **computador desktop**, baseado em uma **arquitetura cliente/servidor**. A comunicação deverá ocorrer em rede, integrando **Arduino IDE (cliente)** e **Python (servidor)**.

O sistema deverá simular a segurança de uma residência, incluindo ativação do alarme, detecção de intrusão, autenticação de usuários via rede e notificações em caso de falha de autenticação.

---

## 2. Arquitetura do Sistema

* **Cliente**:

  * Executado no **ESP32**
  * Responsável pelo controle dos sensores e atuadores
  * Envio das senhas digitadas pelo usuário para o servidor
  * Basear-se no arquivo `WiFiClientModificado.ino`

* **Servidor**:

  * Executado no **desktop**, em Python
  * Responsável pela autenticação de usuários
  * Armazenamento de usuários e senhas em arquivo
  * Envio de respostas de autenticação ao cliente
  * Basear-se nos arquivos:

    * `websocketserver.py`
    * `testeEmail.py`

---

## 3. Componentes Utilizados

Todos os componentes eletrônicos devem estar conectados ao **ESP32**:

* Botão (ativação do alarme)
* LED
* Buzzer
* Servo motor
* Sensor de luminosidade (LDR)
* Sensor de toque
* Display de 7 segmentos

---

## 4. Funcionamento do Sistema de Alarme

### 4.1 Ativação do Alarme

1. O sistema inicia desativado.
2. O usuário deve **pressionar um botão** para ativar o alarme.
3. Após a ativação:

   * O **LED deve piscar por 10 segundos**, indicando o tempo para o usuário fechar uma porta hipotética.
   * Ao final dos 10 segundos, o LED se apaga e o alarme passa para o estado **ativado**.
4. Um **servo motor deve girar 90 graus**, representando o travamento da porta.

---

### 4.2 Detecção de Intrusão

5. Se o sistema estiver ativado e ocorrer uma intrusão:

   * Considera-se intrusão quando o **sensor de luminosidade** ou o **sensor de toque** for ativado.
   * O **buzzer deve emitir 2 beeps**.

---

### 4.3 Autenticação do Usuário

6. Após a detecção de intrusão:

   * O usuário deve inserir uma **senha pelo monitor serial**.
   * O usuário terá **10 segundos** para digitar a senha.
   * O tempo restante deve ser exibido no **display de 7 segmentos**.

7. A senha digitada no cliente deve ser:

   * Enviada ao **servidor via rede**
   * Validada com base em um arquivo contendo **usuários e senhas** armazenado no servidor

---

### 4.4 Respostas do Servidor

8. O servidor deve:

   * Verificar se a senha informada está cadastrada
   * Caso esteja correta:

     * Enviar o **nome do usuário** ao cliente
   * Caso esteja incorreta:

     * Enviar uma **mensagem de erro** ao cliente

9. No cliente:

   * Se a autenticação for bem-sucedida:

     * O **nome do usuário** deve ser exibido no monitor serial
     * O **servo motor deve retornar à posição inicial**, desativando a trava
   * Se a autenticação falhar:

     * Uma mensagem de erro deve ser exibida
     * O usuário terá **mais 10 segundos** para tentar novamente

---

### 4.5 Falhas de Autenticação

10. Se o usuário **errar a senha duas vezes consecutivas**:

* No cliente:

  * O **buzzer deve emitir um som contínuo**
  * O **LED deve piscar de forma ininterrupta**

* No servidor:

  * Deve ser enviado um **email de alerta**, informando:

    * Data do acesso negado
    * Horário do acesso negado
  * Utilizar o arquivo `testeEmail.py` como base

---

## 5. Requisitos Finais

* Comunicação obrigatoriamente via **rede cliente/servidor**
* Autenticação centralizada no **servidor**
* Controle de sensores e atuadores no **ESP32**
* Código organizado e baseado nos arquivos fornecidos
* Funcionamento completo conforme os estados descritos

---

## 6. Entrega Esperada

* Código do cliente (ESP32)
* Código do servidor (Python)
* Arquivo de usuários e senhas
* Demonstração do funcionamento completo do sistema
