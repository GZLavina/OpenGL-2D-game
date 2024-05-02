# Trabalho GA - Processamento Gráfico

## Gustavo Lavina e Vitor Goulart

## O Jogo

Este trabalho é uma implementação de um jogo 2D em C++ utilizando OpenGL. O objetivo do jogador é controlar o golfinho para coletar o máximo de peixes possível para aumentar seu **SCORE** enquanto evita as latas de lixo que caem na água. O jogador possui 3 vidas, ao colidir com o lixo o jogador perde 1 vida e quando perder todas as vidas o jogo acaba.

## Instruções

- Após clonar o repositório, o que pode ser feito diretamente pela IDE (recomenda-se o Visual Studio Code), configurar o CMake para utilizar o compilador amd64 e clicar em "Build". Depois, basta executar.
- Dependendo da localização do executável gerado pelo CMake, é possível que os caminhos de shaders e texturas estejam incorretos. No entanto, executando pelo VSCode, os caminhos devem estar corretos.
- Em caso de erro relacionado à biblioteca FreeType, usar a versão da branch "no-freetype".

## Organização do Código

### Estrutura de Diretórios

- Dependencies: Armazena os headers e os arquivos .lib das bibliotecas utilizadas;
- Fonts: Armazena as fontes que podem ser carregadas pelo TextRenderer, as fontes devem possuir ser arquivos `.ttf`;
- shaders: Armazena os vertex e fragment shaders utilizados;
- Sprites: Armazena as classes de sprites utilizadas;
- Textures: Armazena as texturas e os *spritesheets* utilizados.

### Classes

- BackgroundLayer: Classe para gerenciar o background e controlar animações para efeito **PARALAX**;
- Fish: Classe para gerenciar os peixes coletáveis no jogo, contendo funções para o controle da aniamção e do surgimento dos peixes;
- Player: Classe para gerenciar o jogador, controlar o movimento, alterar animações e controlar elementos como a vida do jogador;
- Sprite: Classe que implementa funções gerais para a detecção de colisão, renderização e atualização das sprites e atributos necessários para controlar a velocidade da animação, sua posição na tela, seu tamanho e identificador de textura;
  -Trash: Classe para gerenciar a animação e aparição de elementos que causam dano ao jogador no jogo.
