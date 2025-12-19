# 📡 Controller Area Network (CAN Bus)

## 📌 O que é CAN?

O **Controller Area Network (CAN)** é um **protocolo de comunicação serial baseado em mensagens**, criado para permitir que **Unidades de Controle Eletrônico (ECUs)** se comuniquem entre si de forma:

- Confiável
- Determinística
- Prioritária
- Sem a necessidade de um computador mestre (host)

O CAN é amplamente utilizado em:
- Automóveis
- Veículos pesados
- Sistemas industriais
- Aviação
- Robótica

O protocolo é padronizado principalmente pela **ISO 11898**.

---

## 🧠 Conceito-chave: comunicação baseada em mensagens

No CAN:

- ❌ Não existe endereço de origem ou destino
- ✅ Toda mensagem é **broadcast**
- ✅ Todas as ECUs recebem todas as mensagens
- ✅ Cada ECU decide se a mensagem é relevante ou não

A **prioridade** e o **significado** da mensagem são definidos pelo **Identifier**.

---

## ⚡ Características principais do CAN

- Comunicação **multi-master** (qualquer nó pode transmitir)
- Arbitragem por prioridade (sem colisão destrutiva)
- Detecção e tratamento de erros em hardware
- Comunicação em tempo real (determinística)
- Alta imunidade a ruído elétrico

---

## 🔌 Camadas do CAN

O CAN define principalmente:
- **Camada Física** (sinais elétricos)
- **Camada de Enlace** (frames, arbitragem, CRC, ACK)

Camadas superiores (ex: interpretação de dados) são definidas por:
- CANopen
- J1939
- ISO-TP
- UDS
- Proprietários (automotivo)

---

# 🧩 Estrutura de um Frame CAN (Standard – 11 bits)

Um frame CAN é composto por vários campos transmitidos **bit a bit**.

SOF | Identifier | RTR | IDE | R0 | DLC | Data | CRC | ACK | EOF | IFS
---

## 🟢 SOF — Start of Frame (1 bit)

- Sempre **dominante (0)**
- Marca o início de uma nova mensagem
- Sincroniza todos os nós da rede
- Só pode ocorrer após o período de IFS

---

## 🆔 Identifier — Identificador (11 bits)

- Define o **tipo da mensagem**
- Define a **prioridade**
- Quanto **menor o valor**, **maior a prioridade**

Exemplo:
ID 0x100 → maior prioridade
ID 0x300 → menor prioridade

👉 Usado também na **arbitragem** do barramento.

---

## 🔄 RTR — Remote Transmission Request (1 bit)

- **Dominante (0)** → frame de dados
- **Recessivo (1)** → solicitação remota

Usado quando:
- Um nó solicita dados
- Outro nó responde com um frame de dados com o mesmo ID

⚠️ Pouco usado em sistemas modernos.

---

## 🧩 IDE — Identifier Extension (1 bit)

- **Dominante (0)** → CAN padrão (11 bits)
- **Recessivo (1)** → CAN estendido (29 bits)

Este documento trata do **CAN padrão**.

---

## 🧪 R0 — Reserved (1 bit)

- Reservado para uso futuro
- Sempre transmitido como **dominante (0)**

---

## 📏 DLC — Data Length Code (4 bits)

- Define o número de bytes no campo de dados
- Valores possíveis: `0` a `8` (CAN clássico)

Exemplo:

DLC = 8 → 8 bytes de dados


⚠️ DLC ≠ tamanho real em CAN FD (extensão moderna).

---

## 📦 Data Field — Dados (0 a 8 bytes)

- Contém os dados reais da aplicação
- Estrutura definida pela aplicação ou protocolo superior
- Interpretado por:
  - Máscaras
  - Shifts
  - Scaling
  - Offset

Exemplo:

Byte 0–1 → RPM
Byte 2 → Velocidade
Byte 3 → Flags
---

## 🔐 CRC — Cyclic Redundancy Check (16 bits)

- Usado para **detecção de erro**
- Calculado com base em todos os bits anteriores do frame
- Inclui:
  - 15 bits de CRC
  - 1 bit delimitador (recessivo)

Se o CRC não bater:
- O frame é descartado
- Um erro é sinalizado no barramento

---

## 🤝 ACK — Acknowledgement (2 bits)

Composto por:
- **ACK Slot** (1 bit)
- **ACK Delimiter** (1 bit)

Funcionamento:
- Transmissor envia recessivo
- Qualquer nó que recebeu corretamente sobrescreve com dominante
- Se ninguém reconhecer → erro

👉 Isso garante que **alguém recebeu** a mensagem.

---

## 🛑 EOF — End of Frame (7 bits)

- Sempre recessivo (`1`)
- Marca o fim do frame CAN
- Indica que a transmissão terminou corretamente

---

## ⏳ IFS — Inter Frame Space (mínimo 3 bits)

- Intervalo entre frames
- Permite que o controlador:
  - Processe o frame recebido
  - Atualize buffers
- Deve conter **no mínimo 3 bits recessivos consecutivos**

Após isso:
- Um novo bit dominante inicia outro SOF

---

# ⚖️ Arbitragem CAN (conceito essencial)

Quando dois nós transmitem ao mesmo tempo:

- A transmissão ocorre **bit a bit**
- `0` (dominante) sobrescreve `1` (recessivo)
- O nó que transmitir `1` e ler `0` **perde a arbitragem**
- O nó perdedor **para imediatamente** (sem erro)

👉 Resultado:
- Mensagem de **menor ID ganha**
- Nenhum frame é corrompido

---

# 🧠 Resumo mental (importante)

- CAN é **broadcast**
- Identificador define **prioridade e significado**
- Endianness só importa no **campo Data**
- Arbitragem é **não destrutiva**
- Robustez vem do **hardware**

---

# 🚗 Conexão com ECU e Firmware

Na prática, você:
- Recebe bytes
- Aplica máscaras e shifts
- Converte endianness
- Atualiza estado interno da ECU
- Gera novos frames

---

✍️ **Esse documento é base sólida para firmware automotivo.**
