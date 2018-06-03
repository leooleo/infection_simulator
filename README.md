# Simulador de infecção
Trabalho desenvolvido por Léo Moraes da Silva

# Introdução
Este trabalho foi desenvolvido para a matéria de programação concorrente de 2018.
O objetivo do mesmo é desenvolver um simulador onde existem 4 atores(Processos) concorrentes: 

●Macrófagos(Glóbulos brancos); 
●Células(Uma célula qualquer tal qual uma hemácia);
●Produtor de recursos corporais(Qualquer sistema que produz recurso corporal tal qual o digestivo, que libera a glicose no sistema circulatório);
●Bactéria(Agente causador da infecção);
# Formalização do problema proposto
Primeiramente, é necessário definir uma infecção: Uma invasão de tecidos corporais no hospedeiro capaz de gerar doenças. Uma doença infecciosa corresponde a qualquer doença clinicamente evidente que seja o resultado de uma infeção, presença e multiplicação de agentes biológicos patogêncios no organismo hospedeiro(Wikipedia).
No problema proposto, a infecção é bacteriana onde o parasita é a bactéria e o corpo humano o hospedeiro.
Buscando semelhança a um sistema real, o simulador possui dois agentes em constante crescimento: Bactérias e células. Ou seja, ambas estão sempre buscando realizar a mitose (processo pelo qual as células eucarióticas dividem seus cromossomos entre duas células menores do corpo) e, para isso, utilizam recursos corporais tais como: Glicose, Oxigênio, etc. Assim ambas competem pelas condições do ambiente compartilhado.
Como o sistema precisa ser modelado de tal forma que as bactérias precisem de um agente externo para controlá-las(no caso os macrófagos) estas possuem um crescimento mais acelerado que as células. Além disso, o parasita demanda menos recursos corporais que uma célula, visto que seu tamanho é consideravelmente menor e por consequência seu metabolismo também. Desta forma o sitema tende ao equilíbrio.
É importante ressaltar também que ambas as entidades célula e bactéria sofrem caso haja falta de recursos. Se o corpo não atende a esta necessidade elas devem morrer. É necessário lembrar que um parasita não é eficiente se o mesmo mata o corpo de seu hospedeiro antes do desejado, logo deve consumir de forma regulada até que tenha se reproduzido o suficiente.
# Descrição do algoritmo desenvolvido
Para explanar da melhor forma o comportamento do simulador, serão detalhadas as interações entre os agentes que o compõem.
●Interação Produtor-consumidores(Células e bactérias): Para esta interação pode-se pensar numa implementação muito similar ao problema dos canibais, onde um cozinheiro prepara alimentos para um grupo de pessoas. Ambos agentes usam uma área de memória compartilhada: a variável global num_body_resources que contabiliza quantos recursos disponíveis ainda existem. Aqui, o produtor aumenta essa contagem selecionando um valor aleatório entre 10 e 30 e os consumidores decrescem a mesma quando realizam a mitose. Além disso, as células sinalizam quando a contagem de recursos começa a decrescer(quando num_body_resources for menor que 30). Já as bactérias sinalizam assim que morrem pela falta dos mesmos. Para o processo de mitose, o sistema dá uma permissão ao semáforo do agente que está se dividindo, assim uma thread que antes estava em estado de espera agora passa a executar como um agente normal.
●Interação Célula-Bactéria: Como previamente explicado, ambas as entidades competem para atingir um número maior. Porém, pelo metabolismo mais acelerado as bactérias o fazem num ritmo mais veloz. Para tal fim, o simulador aplica sleeps maiores para as células e faz com que essas requeiram mais recursos para fazer a mitose. Assim, existe a tendência de um maior numéro de threads que representam bactérias do que células. A nível de harware, o processador ficará mais ocupado com as threads de maior número garantindo a dominância ao agente que possuir tal característica. Quando um agente morre pela falta de recurso deve voltar ao estado de espera, que representa a morte do mesmo, onde a thread não altera o estado do sistema.
●Interação macrófago-Bactéria: Para garantir equilíbrio ao sistema os macrófagos devem “matar” as bácterias. Para implementação desse requerimento é usada a variável global white_cell_killings que representa quantas bacérias estão sendo mortas pelos glóbulos brancos. Assim, os mácrofagos incremental ela a cada três segundos e as bactérias, ao identificarem que essa váriavel é maior que 0, ou seja, quando existe a execução da ordem de “morte”, devem decrementar o contador e voltar ao estado de espera(que representa a morte da mesma).

No sistema produzido, pode-se alterar a contagem de cada agente, dessa forma manipulando para qual lado o sistema deve pender: ao parasita ou ao hospedeiro. Esses números podem ser acessados nos #define’s no cabeçalho do código, correspondendo às variáveis: num_max_cells, num_max_bacteria, num_white_cells, initial_cells_count e initial_bacteria_count.
Nas definições originais do simulador,  existem dois glóbulos brancos e uma contagem inicial de 6 células e bactérias, sendo tais dados suficientes para o corpo combater a infecção. Porém, removendo os macrófagos, pode-se observar como as células mal conseguem se reproduzir e o oposto ocorrendo com as bactérias.
Para o fim da execução um agente(célula ou bactéria) deve possuir pelo menos o dobro de sua contagem inicial e da contagem de seu competidor. Dessa forma, o ritmo de crescimento de um é o dobro do outro, assegurando a dominância do mesmo sobre os recursos corporais.

# Conclusão
Deve-se agorar, ressaltar que o simulador produzido não reflete de forma real como os agentes que compõem o sistema de fato funcionam. A visão aqui criada é reducionista. Como exemplo disso, não pode-se acreditar que o ritmo de crescimento de bactérias e células é tão simples quanto o realizado nesse simulador. Além disso esses agentes possuem apenas dois estados: Espera e Divisão. É de conhecimento científico que ambos possuem complexidade muito maior que esta envolvendo estados como redução e aumento metabólico entre outros. Além disso bactérias podem se reproduzir por métodos que não a mitose.

# Referências
https://pt.wikipedia.org/wiki/Infec%C3%A7%C3%A3o
https://pt.wikipedia.org/wiki/Bact%C3%A9ria
https://en.wikipedia.org/wiki/Bacterial_growth
https://pt.wikipedia.org/wiki/Macr%C3%B3fago
https://www.youtube.com/watch?v=zQGOcOUBi6s
