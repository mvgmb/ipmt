# ipmt

## Requisitos Mínimos

- Ubuntu 16.04 ou superior
- cmake 3.15 ou superior
- make 4.1 ou superior
- g++ 8.0.0 ou superior

## Compilando

O binário `ipmt` será gerado no diretório `bin` do projeto.

```
cmake .
make
```

## Executando

Por padrão o help do programa é exibido.

```
./bin/ipmt
```

### Exemplo de invocação do programa

```
./bin/ipmt index moby-dick.txt
./bin/ipmt search whale moby-dick.idx

./bin/ipmt zip moby-dick.txt
./bin/ipmt unzip moby-dick.txt
```
