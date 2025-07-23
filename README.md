# Simulador de Cache

# Alunos:
Bernardo Robaina - M2
William Pavinato - M2

Este projeto é um **simulador de cache parametrizável**, capaz de analisar o comportamento de diferentes configurações de cache, incluindo políticas de substituição e mapeamento.

## ✅ Funcionalidades

- Leitura de endereços binários (32 bits) em formato **big-endian**.
- Suporte a:
  - Mapeamento direto
  - Mapeamento associativo por conjunto
  - Cache totalmente associativa
- Políticas de substituição:
  - FIFO (First-In, First-Out)
  - LRU (Least Recently Used)
  - Random (aleatória)
- Contabiliza:
  - Acessos totais
  - Hits
  - Misses (compulsório, conflito, capacidade)

## ⚙️ Como compilar

Use qualquer compilador C. Exemplo com `gcc`:

```bash
gcc -o simulador_cache simulador_cache.c
```

## ▶️ Como executar

```bash
./simulador_cache <n_sets> <assoc> <block_size> <replacement_policy> <input_file>
```

### Parâmetros:

| Parâmetro              | Descrição                                       |
|------------------------|-------------------------------------------------|
| `<n_sets>`             | Número de conjuntos (sets)                     |
| `<assoc>`              | Grau de associatividade (vias por conjunto)    |
| `<block_size>`         | Tamanho do bloco (em bytes)                    |
| `<replacement_policy>` | Política de substituição: `F` (FIFO), `L` (LRU), `R` (Random) |
| `<input_file>`         | Caminho para o arquivo com endereços binários  |

### Exemplo:

```bash
./simulador_cache 128 2 16 L 'bin_100.bin'
```

Executa o simulador com:
- 128 conjuntos
- 2 vias por conjunto (associatividade 2-way)
- Bloco de 16 bytes
- Política de substituição LRU

## 📊 Saída

Ao final da execução, o simulador exibe:

- Total de acessos
- Total de hits
- Total de misses
  - Misses compulsórios
  - Misses por conflito
  - Misses por capacidade