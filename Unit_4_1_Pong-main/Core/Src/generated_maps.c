/*
 * Generated/static map data from Tiled .tmj exports.
 * PC-side conversion only: the STM32 consumes these static const arrays.
 * Collision layers are exported as compact boolean masks: 0 = passable, 1 = blocked.
 * game.c still treats out-of-bounds map coordinates as solid.
 */

#include "generated_maps.h"

#include <stdint.h>

/* OVERWORLD: 32x21, Tiled tile size 32x32. Source: maps/overworld.tmj. */
/* Compact tile remap:
 *   0 means empty/no tile.
 *     1 -> Tiled GID 76
 *     2 -> Tiled GID 77
 *     3 -> Tiled GID 87
 *     4 -> Tiled GID 88
 *     5 -> Tiled GID 98
 *     6 -> Tiled GID 99
 *     7 -> Tiled GID 119
 *     8 -> Tiled GID 120
 *     9 -> Tiled GID 121
 *     10 -> Tiled GID 130
 *     11 -> Tiled GID 131
 *     12 -> Tiled GID 132
 *     13 -> Tiled GID 179
 *     14 -> Tiled GID 180
 *     15 -> Tiled GID 181
 *     16 -> Tiled GID 185
 *     17 -> Tiled GID 186
 *     18 -> Tiled GID 187
 *     19 -> Tiled GID 190
 *     20 -> Tiled GID 191
 *     21 -> Tiled GID 192
 *     22 -> Tiled GID 196
 *     23 -> Tiled GID 197
 *     24 -> Tiled GID 198
 *     25 -> Tiled GID 199
 *     26 -> Tiled GID 200
 *     27 -> Tiled GID 201
 *     28 -> Tiled GID 202
 *     29 -> Tiled GID 203
 *     30 -> Tiled GID 211
 *     31 -> Tiled GID 212
 *     32 -> Tiled GID 213
 *     33 -> Tiled GID 214
 *     34 -> Tiled GID 215
 *     35 -> Tiled GID 224
 *     36 -> Tiled GID 225
 *     37 -> Tiled GID 226
 *     38 -> Tiled GID 228
 *     39 -> Tiled GID 229
 *     40 -> Tiled GID 230
 *     41 -> Tiled GID 234
 *     42 -> Tiled GID 239
 *     43 -> Tiled GID 240
 *     44 -> Tiled GID 241
 *     45 -> Tiled GID 242
 *     46 -> Tiled GID 247
 *     47 -> Tiled GID 248
 *     48 -> Tiled GID 249
 *     49 -> Tiled GID 260
 *     50 -> Tiled GID 261
 *     51 -> Tiled GID 305
 *     52 -> Tiled GID 320
 *     53 -> Tiled GID 332
 *     54 -> Tiled GID 334
 *     55 -> Tiled GID 335
 *     56 -> Tiled GID 336
 *     57 -> Tiled GID 338
 *     58 -> Tiled GID 339
 *     59 -> Tiled GID 340
 *     60 -> Tiled GID 344
 *     61 -> Tiled GID 430
 *     62 -> Tiled GID 431
 *     63 -> Tiled GID 455
 *     64 -> Tiled GID 456
 *     65 -> Tiled GID 466
 *     66 -> Tiled GID 467
 *     67 -> Tiled GID 468
 *     68 -> Tiled GID 485
 *     69 -> Tiled GID 486
 *     70 -> Tiled GID 491
 *     71 -> Tiled GID 492
 *     72 -> Tiled GID 493
 *     73 -> Tiled GID 495
 *     74 -> Tiled GID 503
 *     75 -> Tiled GID 504
 *     76 -> Tiled GID 505
 *     77 -> Tiled GID 522
 *     78 -> Tiled GID 523
 *     79 -> Tiled GID 529
 *     80 -> Tiled GID 540
 *     81 -> Tiled GID 556
 *     82 -> Tiled GID 557
 *     83 -> Tiled GID 558
 *     84 -> Tiled GID 566
 *     85 -> Tiled GID 577
 *     86 -> Tiled GID 593
 *     87 -> Tiled GID 594
 *     88 -> Tiled GID 595
 *     89 -> Tiled GID 645
 *     90 -> Tiled GID 666
 *     91 -> Tiled GID 703
 *     92 -> Tiled GID 713
 *     93 -> Tiled GID 722
 *     94 -> Tiled GID 740
 *     95 -> Tiled GID 750
 *     96 -> Tiled GID 751
 *     97 -> Tiled GID 752
 *     98 -> Tiled GID 753
 *     99 -> Tiled GID 757
 *     100 -> Tiled GID 758
 *     101 -> Tiled GID 759
 *     102 -> Tiled GID 761
 *     103 -> Tiled GID 762
 *     104 -> Tiled GID 763
 *     105 -> Tiled GID 764
 *     106 -> Tiled GID 765
 *     107 -> Tiled GID 766
 *     108 -> Tiled GID 767
 *     109 -> Tiled GID 777
 *     110 -> Tiled GID 787
 *     111 -> Tiled GID 788
 *     112 -> Tiled GID 789
 *     113 -> Tiled GID 790
 *     114 -> Tiled GID 794
 *     115 -> Tiled GID 795
 *     116 -> Tiled GID 796
 *     117 -> Tiled GID 798
 *     118 -> Tiled GID 799
 *     119 -> Tiled GID 800
 *     120 -> Tiled GID 801
 *     121 -> Tiled GID 802
 *     122 -> Tiled GID 803
 *     123 -> Tiled GID 804
 *     124 -> Tiled GID 814
 *     125 -> Tiled GID 820
 *     126 -> Tiled GID 831
 *     127 -> Tiled GID 832
 *     128 -> Tiled GID 833
 *     129 -> Tiled GID 835
 *     130 -> Tiled GID 836
 *     131 -> Tiled GID 837
 *     132 -> Tiled GID 838
 *     133 -> Tiled GID 839
 *     134 -> Tiled GID 840
 *     135 -> Tiled GID 841
 *     136 -> Tiled GID 851
 *     137 -> Tiled GID 857
 *     138 -> Tiled GID 868
 *     139 -> Tiled GID 869
 *     140 -> Tiled GID 870
 *     141 -> Tiled GID 871
 *     142 -> Tiled GID 899
 *     143 -> Tiled GID 900
 *     144 -> Tiled GID 901
 *     145 -> Tiled GID 905
 *     146 -> Tiled GID 906
 *     147 -> Tiled GID 907
 *     148 -> Tiled GID 908
 *     149 -> Tiled GID 936
 *     150 -> Tiled GID 937
 *     151 -> Tiled GID 938
 *     152 -> Tiled GID 945
 *     153 -> Tiled GID 973
 *     154 -> Tiled GID 974
 *     155 -> Tiled GID 975
 *     156 -> Tiled GID 982
 */
static const uint8_t overworld_ground[] = {
     41u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,  47u,  27u,  28u, 103u, 104u, 105u, 106u, 107u,  26u,  27u,  48u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,
     25u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  49u,  64u,  33u, 118u, 119u, 120u, 121u, 122u,  31u,  64u,  50u,  27u,  27u,  27u,  27u,  29u,  41u,  41u,  41u,  41u,  41u,  41u,
     30u,  38u,  39u,  39u,  40u,  32u,  32u,  32u,  32u,  32u,  64u,  63u,  37u, 130u, 131u, 132u, 133u, 134u,  35u,  63u,  64u,  32u,  32u,  32u,  32u,  42u,  41u,  41u,  41u,  41u,  41u,  41u,
     30u,  43u,  44u,  44u,  45u,  36u,  36u,  36u,  36u,  36u,  21u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  36u,  36u,  36u,  36u,  50u,  27u,  27u,  27u,  27u,  27u,  27u,
     30u,  15u,  13u,  14u,  15u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  13u,  14u,  30u,  32u,  32u,  32u,  32u,  32u,  32u,
     30u,  21u,  19u,  20u,  21u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  14u,  15u,  19u,  20u,  30u,  36u,  36u,  36u,  36u,  36u,  36u,
     30u,  15u,  13u,  14u,  15u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  13u,  14u,  30u,  16u,  17u,  18u,  16u,  17u,  18u,
     30u,  21u,  19u,  20u,  21u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  14u,  15u,  13u,  14u,  15u,  19u,  20u,  30u,  22u,  23u,  24u,  22u,  23u,  24u,
     30u,  15u,  13u,  14u,  15u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  19u,  20u,  21u,  13u,  14u,  15u,  16u,  17u,  18u,  16u,  17u,  18u,
     30u,  21u,  19u,  20u,  21u,  25u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  29u,  13u,  14u,  15u,  19u,  20u,  21u,  22u,  23u,  24u,  22u,  23u,  24u,
     46u,  15u,  13u,  14u,  73u,  30u,  32u,  32u,  32u,  32u,  32u,  32u,  32u,  32u,  32u,  32u,  32u,  32u,  32u,  34u,  19u,  20u,  21u,  13u,  14u,  15u,  16u,  17u,  18u,  16u,  17u,  18u,
     20u,  21u,  19u,  20u,  21u,  30u,  36u,  36u,  36u,  36u,  36u,  36u,  36u,  36u,  36u,  36u,  36u,  36u,  36u,  34u,  13u,  14u,  15u,  19u,  20u,  21u,  22u,  23u,  24u,  22u,  23u,  24u,
     14u,  15u,  13u,  14u,  15u,  30u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  34u,  19u,  20u,  21u,  13u,  14u,  25u,  27u,  27u,  27u,  27u,  27u,  27u,
     20u,  21u,  19u,  20u,  21u,  30u,   7u,   8u,   9u,   7u,   8u,   9u,   7u,   8u,   9u,   7u,   8u,   9u,   7u,   8u,  13u,  14u,  15u,  19u,  20u,  30u,  52u,  57u,  58u,  58u,  59u,  52u,
     14u,  15u,  13u,  14u,  15u,  30u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  19u,  20u,  21u,  13u,  25u,  49u,  53u,  54u,  55u,  55u,  56u,  53u,
     46u,  21u,  19u,  20u,  21u,  30u,   7u,   8u,   9u,   7u,   8u,   9u,   7u,   8u,   9u,   7u,   8u,   9u,   7u,   8u,  13u,  14u,  15u,  19u,  30u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,
     50u,  27u,  27u,  27u,  27u,  51u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  19u,  20u,  21u,  13u,  30u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,
     57u,  58u,  58u,  58u,  59u,  34u,   7u,   8u,   9u,   7u,   8u,   9u,   7u,   8u,   9u,   7u,   8u,   9u,   7u,  42u,  13u,  14u,  15u,  19u,  30u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,
     54u,  55u,  55u,  55u,  56u,  34u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  11u,  12u,  10u,  34u,  19u,  20u,  21u,  13u,  30u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,
     41u,  41u,  41u,  41u,  41u,  50u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  27u,  49u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,
     41u,  41u,  41u,  41u,  41u,  52u,  57u,  58u,  58u,  59u,  60u,  60u,  60u,  57u,  58u,  58u,  58u,  59u,  60u,  60u,  57u,  58u,  58u,  59u,  52u,  41u,  41u,  41u,  41u,  41u,  41u,  41u,
};
static const uint8_t overworld_objects[] = {
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 102u,   0u,   0u,   0u,   0u,   0u, 108u,  70u,  72u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 117u,   0u,   0u,   0u,   0u,   0u, 123u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,  89u,   0u,   0u,   0u,  33u,   0u, 129u,   0u,   0u,   0u,   0u,   0u, 135u,   0u,  31u,  99u, 100u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,  96u,  97u,  98u,  37u,   0u,   0u, 137u, 137u, 125u, 137u, 125u,   0u,   0u,  35u, 114u, 115u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u, 141u,   0u,   0u,   0u,   0u,   0u, 111u, 112u, 113u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 126u, 127u,   0u,   0u,   0u,   0u,  99u, 100u,   0u,  70u,  79u,
      0u, 148u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 114u, 115u,   0u,  68u,  69u,
      0u, 152u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 138u, 139u, 140u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 126u, 127u,   0u,  77u,  78u,
      0u, 156u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 145u, 146u, 147u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 125u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,  61u,  62u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 137u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
     90u,   0u,   0u,  66u,  67u,   0u,  70u,  71u,   0u, 138u, 139u, 140u,   0u, 142u, 143u, 144u, 142u, 143u, 144u,   0u,   0u,   0u,   0u,   0u, 125u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
     91u, 137u,   0u,  75u,  76u,   0u,  81u,  82u,  83u, 145u, 146u, 147u,   0u, 149u, 150u, 151u, 149u, 150u, 151u,   0u,   0u,   0u,   0u,   0u, 137u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
     94u, 125u,   0u,   0u,  93u,   0u,  86u,  87u,  88u,   0u,   0u,   0u,   0u, 153u, 154u, 155u, 153u, 154u, 155u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
    109u, 137u,   0u,   0u, 101u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   2u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
    124u, 125u,   0u,   0u, 116u,   0u, 141u,   0u,   0u,  92u,   0u,  92u,   0u,  92u,   0u,  92u,   0u,   0u,   3u,   0u,   4u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
    136u,   0u,   0u,   0u, 128u,   0u, 148u,   0u,   0u,  95u,   0u,  95u,   0u,  95u,   0u,  95u,   0u,   0u,   3u,   0u,   4u,   0u,   0u,  65u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u, 152u,   0u,   0u, 110u,   0u, 110u,   0u, 110u,   0u, 110u,   0u,   0u,   5u,   0u,   6u,   0u,   0u,  74u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,  70u,  84u,   0u,   0u,   0u, 156u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  80u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  85u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
};
static const uint8_t overworld_collision[] = {
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   1u,   1u,   0u,   0u,   0u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      1u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      1u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      1u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      1u,   0u,   0u,   0u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   0u,   0u,   0u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   0u,   0u,   0u,   1u,   1u,   0u,   0u,   0u,   1u,   0u,   1u,   0u,   1u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   0u,   0u,   0u,   1u,   1u,   0u,   0u,   0u,   1u,   0u,   1u,   0u,   1u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   1u,   0u,   1u,   0u,   1u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
};
static const GeneratedEntity_t overworld_entities[] = {
    {GENERATED_ENTITY_NONE, 25u, 8u, LEVEL_COMPLETE}, /* removed entrance from old three-level flow */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 1u, 11u, LEVEL_SPEAKER}, /* LV2 entrance */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 13u, 3u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 14u, 3u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 15u, 3u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 16u, 3u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 17u, 3u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 18u, 3u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 13u, 4u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 14u, 4u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 15u, 4u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 16u, 4u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 17u, 4u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_MODULE_ENTRANCE, 18u, 4u, LEVEL_DISPLAY_BOSS}, /* entrance_display */
    {GENERATED_ENTITY_PLAYER_SPAWN, 9u, 13u, LEVEL_COMPLETE}, /* spawnloc */
    {GENERATED_ENTITY_PLAYER_SPAWN, 22u, 9u, LEVEL_COMPLETE}, /* landingback */
    {GENERATED_ENTITY_PLAYER_SPAWN, 2u, 12u, LEVEL_COMPLETE}, /* landingback1 */
};
/* SPEAKER: 28x28, Tiled tile size 32x32. */
/* Compact tile remap:
 *   0 means empty/no tile.
 *     1 -> Tiled GID 1
 *     2 -> Tiled GID 3
 *     3 -> Tiled GID 5
 *     4 -> Tiled GID 13
 *     5 -> Tiled GID 15
 *     6 -> Tiled GID 17
 *     7 -> Tiled GID 27
 *     8 -> Tiled GID 50
 *     9 -> Tiled GID 51
 *    10 -> Tiled GID 54
 *    11 -> Tiled GID 55
 *    12 -> Tiled GID 56
 *    13 -> Tiled GID 81
 *    14 -> Tiled GID 82
 *    15 -> Tiled GID 101
 *    16 -> Tiled GID 110
 *    17 -> Tiled GID 122
 *    18 -> Tiled GID 134
 *    19 -> Tiled GID 136
 *    20 -> Tiled GID 137
 *    21 -> Tiled GID 138
 *    22 -> Tiled GID 140
 *    23 -> Tiled GID 141
 *    24 -> Tiled GID 142
 *    25 -> Tiled GID 146
 *    26 -> Tiled GID 158
 *    27 -> Tiled GID 232
 *    28 -> Tiled GID 233
 *    29 -> Tiled GID 268
 *    30 -> Tiled GID 269
 *    31 -> Tiled GID 270
 *    32 -> Tiled GID 292
 *    33 -> Tiled GID 293
 *    34 -> Tiled GID 305
 *    35 -> Tiled GID 306
 *    36 -> Tiled GID 307
 *    37 -> Tiled GID 329
 *    38 -> Tiled GID 342
 *    39 -> Tiled GID 358
 *    40 -> Tiled GID 359
 *    41 -> Tiled GID 360
 *    42 -> Tiled GID 361
 *    43 -> Tiled GID 362
 *    44 -> Tiled GID 368
 *    45 -> Tiled GID 379
 *    46 -> Tiled GID 395
 *    47 -> Tiled GID 396
 *    48 -> Tiled GID 397
 *    49 -> Tiled GID 398
 *    50 -> Tiled GID 399
 *    51 -> Tiled GID 445
 *    52 -> Tiled GID 468
 *    53 -> Tiled GID 472
 *    54 -> Tiled GID 473
 *    55 -> Tiled GID 474
 *    56 -> Tiled GID 475
 *    57 -> Tiled GID 505
 *    58 -> Tiled GID 509
 *    59 -> Tiled GID 510
 *    60 -> Tiled GID 511
 *    61 -> Tiled GID 512
 *    62 -> Tiled GID 515
 *    63 -> Tiled GID 524
 *    64 -> Tiled GID 542
 *    65 -> Tiled GID 546
 *    66 -> Tiled GID 547
 *    67 -> Tiled GID 548
 *    68 -> Tiled GID 549
 *    69 -> Tiled GID 552
 *    70 -> Tiled GID 553
 *    71 -> Tiled GID 554
 *    72 -> Tiled GID 555
 *    73 -> Tiled GID 559
 *    74 -> Tiled GID 560
 *    75 -> Tiled GID 561
 *    76 -> Tiled GID 579
 *    77 -> Tiled GID 589
 *    78 -> Tiled GID 590
 *    79 -> Tiled GID 591
 *    80 -> Tiled GID 592
 *    81 -> Tiled GID 596
 *    82 -> Tiled GID 597
 *    83 -> Tiled GID 598
 *    84 -> Tiled GID 616
 *    85 -> Tiled GID 626
 *    86 -> Tiled GID 627
 *    87 -> Tiled GID 628
 *    88 -> Tiled GID 633
 *    89 -> Tiled GID 634
 *    90 -> Tiled GID 635
 *    91 -> Tiled GID 653
 *    92 -> Tiled GID 661
 *    93 -> Tiled GID 663
 *    94 -> Tiled GID 664
 *    95 -> Tiled GID 665
 *    96 -> Tiled GID 670
 *    97 -> Tiled GID 671
 *    98 -> Tiled GID 672
 *    99 -> Tiled GID 673
 *   100 -> Tiled GID 698
 *   101 -> Tiled GID 701
 *   102 -> Tiled GID 702
 *   103 -> Tiled GID 703
 *   104 -> Tiled GID 707
 *   105 -> Tiled GID 708
 *   106 -> Tiled GID 709
 *   107 -> Tiled GID 710
 *   108 -> Tiled GID 738
 *   109 -> Tiled GID 739
 *   110 -> Tiled GID 740
 *   111 -> Tiled GID 747
 *   112 -> Tiled GID 772
 *   113 -> Tiled GID 775
 *   114 -> Tiled GID 776
 *   115 -> Tiled GID 777
 *   116 -> Tiled GID 784
 *   117 -> Tiled GID 809
 *   118 -> Tiled GID 810
 *   119 -> Tiled GID 811
 *   120 -> Tiled GID 812
 *   121 -> Tiled GID 813
 *   122 -> Tiled GID 820
 *   123 -> Tiled GID 825
 *   124 -> Tiled GID 831
 *   125 -> Tiled GID 836
 *   126 -> Tiled GID 842
 *   127 -> Tiled GID 847
 *   128 -> Tiled GID 853
 *   129 -> Tiled GID 858
 *   130 -> Tiled GID 864
 *   131 -> Tiled GID 865
 *   132 -> Tiled GID 866
 *   133 -> Tiled GID 867
 *   134 -> Tiled GID 868
 *   135 -> Tiled GID 869
 *   136 -> Tiled GID 903
 *   137 -> Tiled GID 904
 *   138 -> Tiled GID 905
 *   139 -> Tiled GID 914
 *   140 -> Tiled GID 915
 *   141 -> Tiled GID 916
 *   142 -> Tiled GID 964
 *   143 -> Tiled GID 967
 *   144 -> Tiled GID 971
 *   145 -> Tiled GID 975
 */
static const uint8_t speaker_ground[] = {
      1u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   2u,   3u,
      4u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   5u,   6u,
      4u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   7u,   6u,
      4u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u,   6u,
      4u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u,   6u,
      4u, 136u, 137u, 138u, 136u, 137u, 138u, 117u, 118u, 119u, 120u, 121u, 117u, 118u, 119u, 120u, 117u, 118u, 119u, 117u, 118u, 119u, 120u, 121u, 138u, 136u, 137u,   6u,
      4u, 139u, 140u, 141u, 139u, 140u, 141u, 142u, 142u, 142u, 142u, 142u, 142u, 142u, 140u, 141u, 139u, 140u, 142u, 142u, 142u, 142u, 142u, 142u, 141u, 139u, 140u,   6u,
      4u, 136u, 137u, 138u, 136u, 137u, 138u, 145u, 145u, 145u, 145u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 142u, 142u, 142u, 142u, 138u, 136u, 137u,   6u,
      4u, 139u, 140u, 141u, 139u, 140u, 141u, 142u, 142u, 142u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 145u, 145u, 145u, 145u, 141u, 139u, 140u,   6u,
      4u, 136u, 137u, 138u, 136u, 137u, 138u, 145u, 145u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 142u, 138u, 136u, 137u,   6u,
      4u, 139u, 140u, 141u, 139u, 140u, 141u, 142u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 145u, 141u, 139u, 140u,   6u,
     13u, 136u, 137u, 138u, 136u, 137u, 138u, 145u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 145u, 138u, 136u, 137u,   6u,
    141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u,   6u,
    138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u,   6u,
    141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u,   6u,
    138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u,   6u,
     14u, 139u, 140u, 141u, 139u, 140u, 141u, 142u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 142u, 141u, 139u, 140u,   6u,
      4u, 136u, 137u, 138u, 136u, 137u, 138u, 142u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 142u, 138u, 136u, 137u,   6u,
      4u, 139u, 140u, 141u, 139u, 140u, 141u, 145u, 142u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 142u, 145u, 141u, 139u, 140u,   6u,
      4u, 136u, 137u, 138u, 136u, 137u, 138u, 142u, 142u, 142u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 142u, 142u, 142u, 138u, 136u, 137u,   6u,
      4u, 139u, 140u, 141u, 139u, 140u, 141u, 142u, 142u, 142u, 142u, 142u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 142u, 142u, 145u, 142u, 142u, 141u, 139u, 140u,   6u,
      4u, 136u, 137u, 138u, 136u, 137u, 138u, 145u, 145u, 145u, 145u, 145u, 142u, 142u, 137u, 138u, 136u, 142u, 142u, 142u, 145u, 142u, 145u, 145u, 138u, 136u, 137u,   6u,
      4u, 139u, 140u, 141u, 139u, 140u, 141u, 130u, 131u, 132u, 133u, 134u, 135u, 130u, 131u, 132u, 133u, 134u, 135u, 130u, 131u, 132u, 133u, 134u, 141u, 139u, 140u,   6u,
      4u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u, 138u, 136u, 137u,   6u,
      4u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u, 141u, 139u, 140u,   6u,
      8u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  16u,  13u,   0u,   0u,  14u,  16u,   9u,
     25u,  25u,  17u,  17u,  22u,  23u,  23u,  23u,  24u,  17u,  17u,  25u,  25u,  17u,  17u,  22u,  23u,  23u,  23u,  24u,  17u,  17u,  25u,  24u,  17u,  25u,  25u,  25u,
     26u,  26u,  18u,  18u,  19u,  20u,  20u,  20u,  21u,  18u,  18u,  26u,  26u,  18u,  18u,  19u,  20u,  20u,  20u,  21u,  18u,  18u,  26u,  21u,  18u,  26u,  26u,  26u,
};
static const uint8_t speaker_objects[] = {
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,  33u,  44u,  51u,   0u,   0u, 101u, 102u, 103u, 101u, 102u, 103u,  73u,  74u,   0u,  32u,   0u,   0u,   0u,  73u,  74u,   0u,   0u,  10u,  11u,  12u,  12u,   0u,
      0u,   0u,   0u,  70u,  71u,  72u, 108u, 109u, 110u, 108u, 109u, 110u,  81u,  82u,   0u,  37u,  96u,  97u,  98u,  81u,  82u,   0u,  39u,  40u,  41u,  42u,  43u,   0u,
      0u,   0u,   0u,  78u,  79u,  80u, 113u, 114u, 115u, 113u, 114u, 115u,  88u,  89u,   0u,   0u, 104u, 105u, 106u,  88u,  89u,   0u,  46u,  47u,  48u,  49u,  50u,   0u,
      0u,  99u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u, 107u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  27u,  28u,   0u,
      0u, 111u,   0u,   0u,   0u,   0u,   0u, 122u,   0u,   0u,   0u,   0u,   0u,   0u, 143u, 143u, 143u, 143u,   0u,   0u,   0u,   0u,   0u, 123u,   0u,  30u,  31u,   0u,
      0u, 116u,   0u,   0u,   0u,   0u,   0u, 124u,   0u,   0u,   0u, 143u, 143u, 143u,  15u,  15u,  15u, 143u, 143u, 143u,   0u,   0u,   0u, 125u,   0u,  35u,  36u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u, 126u,   0u,   0u, 143u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u, 143u,   0u,   0u, 127u,   0u,   0u,  63u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u, 128u,   0u, 143u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u, 143u, 143u, 123u,   0u,   0u,  75u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u, 122u, 143u,  15u,  15u,  15u,  15u, 144u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u, 125u,   0u,   0u,  83u,   0u,
     52u,  92u,   0u,   0u,   0u,   0u,   0u, 124u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u, 127u,   0u,   0u,  90u,   0u,
     57u, 100u,   0u,   0u,   0u,   0u,   0u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u,   0u,   0u,  99u,   0u,
     64u, 100u,   0u,   0u,   0u,   0u,   0u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 144u,  15u,  15u,  15u,  15u,  15u, 143u,   0u,   0u, 107u,   0u,
     76u, 100u,   0u,   0u,   0u,   0u,   0u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u,   0u,   0u, 111u,   0u,
     84u, 100u,   0u,   0u,   0u,   0u,   0u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u,   0u,   0u, 116u,   0u,
     91u, 112u,   0u,   0u,   0u,   0u,   0u, 124u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u, 123u,   0u,   0u,   0u,   0u,
      0u,  99u,   0u,   0u,   0u,   0u,   0u, 126u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u, 125u,   0u,   0u,   0u,   0u,
      0u, 107u,   0u,   0u,   0u,   0u,   0u, 122u,   0u, 143u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u,   0u, 123u,   0u,   0u,  29u,   0u,
      0u, 111u,   0u,   0u,   0u,   0u,   0u, 124u,   0u,   0u, 143u, 143u,  15u,  15u,  15u,  15u,  15u,  15u,  15u, 143u, 143u,   0u,   0u, 125u,   0u,   0u,  34u,   0u,
      0u, 116u,   0u,   0u,   0u,   0u,   0u, 126u,   0u,   0u,   0u,   0u, 143u, 143u,  15u,  15u,  15u, 143u, 143u,   0u,   0u,   0u,   0u, 127u,   0u,   0u,  38u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u, 128u,   0u,   0u,   0u,   0u,   0u,   0u, 143u, 143u, 143u,   0u,   0u,  62u,   0u,  62u,   0u, 129u,   0u,   0u,  45u,   0u,
      0u,   0u,  85u,  86u,  87u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  69u,   0u,  69u,   0u,   0u,   0u,   0u,  34u,   0u,
      0u,   0u,  93u,  94u,  95u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  77u,   0u,  77u,   0u,   0u,   0u,   0u,  38u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  45u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  53u,  54u,  55u,  56u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  58u,  59u,  60u,  61u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  65u,  66u,  67u,  68u,   0u,   0u,
};
static const uint8_t speaker_collision[] = {
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   0u,   0u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   1u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   1u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
};
static const GeneratedEntity_t speaker_entities[] = {
    {GENERATED_ENTITY_SPEAKER_LINT, 13u, 12u, LEVEL_COMPLETE}, /* lint#1 */
    {GENERATED_ENTITY_SPEAKER_LINT, 19u, 13u, LEVEL_COMPLETE}, /* lint#2 */
    {GENERATED_ENTITY_SPEAKER_LINT, 14u, 17u, LEVEL_COMPLETE}, /* lint#3 */
    {GENERATED_ENTITY_SPEAKER_LINT, 16u, 14u, LEVEL_COMPLETE}, /* lint#4 */
    {GENERATED_ENTITY_SPEAKER_LINT, 9u, 16u, LEVEL_COMPLETE}, /* lint#5 */
    {GENERATED_ENTITY_MODULE_EXIT, 0u, 11u, LEVEL_COMPLETE}, /* doorback1 */
    {GENERATED_ENTITY_PLAYER_SPAWN, 3u, 12u, LEVEL_COMPLETE}, /* landing1 */
    {GENERATED_ENTITY_FAULT_NODE, 19u, 12u, LEVEL_SPEAKER}, /* speaker_obj2 */
    {GENERATED_ENTITY_FAULT_NODE, 14u, 16u, LEVEL_SPEAKER}, /* speaker_obj3 */
    {GENERATED_ENTITY_FAULT_NODE, 9u, 15u, LEVEL_SPEAKER}, /* speaker_obj5 */
    {GENERATED_ENTITY_FAULT_NODE, 13u, 10u, LEVEL_SPEAKER}, /* speaker_obj1 */
    {GENERATED_ENTITY_FAULT_NODE, 16u, 13u, LEVEL_SPEAKER}, /* speaker_obj4 */
};

/* DISPLAY: 32x32, Tiled tile size 32x32. */
/* Compact tile remap:
 *   0 means empty/no tile.
 *     1 -> Tiled GID 1
 *     2 -> Tiled GID 2
 *     3 -> Tiled GID 3
 *     4 -> Tiled GID 4
 *     5 -> Tiled GID 5
 *     6 -> Tiled GID 13
 *     7 -> Tiled GID 14
 *     8 -> Tiled GID 15
 *     9 -> Tiled GID 16
 *    10 -> Tiled GID 17
 *    11 -> Tiled GID 26
 *    12 -> Tiled GID 27
 *    13 -> Tiled GID 28
 *    14 -> Tiled GID 30
 *    15 -> Tiled GID 31
 *    16 -> Tiled GID 32
 *    17 -> Tiled GID 42
 *    18 -> Tiled GID 43
 *    19 -> Tiled GID 44
 *    20 -> Tiled GID 50
 *    21 -> Tiled GID 51
 *    22 -> Tiled GID 54
 *    23 -> Tiled GID 83
 *    24 -> Tiled GID 119
 *    25 -> Tiled GID 122
 *    26 -> Tiled GID 123
 *    27 -> Tiled GID 134
 *    28 -> Tiled GID 135
 *    29 -> Tiled GID 136
 *    30 -> Tiled GID 137
 *    31 -> Tiled GID 138
 *    32 -> Tiled GID 140
 *    33 -> Tiled GID 141
 *    34 -> Tiled GID 142
 *    35 -> Tiled GID 146
 *    36 -> Tiled GID 148
 *    37 -> Tiled GID 158
 *    38 -> Tiled GID 160
 *    39 -> Tiled GID 377
 *    40 -> Tiled GID 378
 *    41 -> Tiled GID 379
 *    42 -> Tiled GID 388
 *    43 -> Tiled GID 389
 *    44 -> Tiled GID 390
 *    45 -> Tiled GID 489
 *    46 -> Tiled GID 490
 *    47 -> Tiled GID 491
 *    48 -> Tiled GID 493
 *    49 -> Tiled GID 556
 *    50 -> Tiled GID 557
 *    51 -> Tiled GID 558
 *    52 -> Tiled GID 593
 *    53 -> Tiled GID 594
 *    54 -> Tiled GID 595
 *    55 -> Tiled GID 643
 *    56 -> Tiled GID 681
 *    57 -> Tiled GID 682
 *    58 -> Tiled GID 713
 *    59 -> Tiled GID 717
 *    60 -> Tiled GID 718
 *    61 -> Tiled GID 719
 *    62 -> Tiled GID 722
 *    63 -> Tiled GID 750
 *    64 -> Tiled GID 751
 *    65 -> Tiled GID 752
 *    66 -> Tiled GID 753
 *    67 -> Tiled GID 754
 *    68 -> Tiled GID 755
 *    69 -> Tiled GID 756
 *    70 -> Tiled GID 757
 *    71 -> Tiled GID 758
 *    72 -> Tiled GID 759
 *    73 -> Tiled GID 761
 *    74 -> Tiled GID 762
 *    75 -> Tiled GID 763
 *    76 -> Tiled GID 764
 *    77 -> Tiled GID 765
 *    78 -> Tiled GID 766
 *    79 -> Tiled GID 767
 *    80 -> Tiled GID 787
 *    81 -> Tiled GID 788
 *    82 -> Tiled GID 789
 *    83 -> Tiled GID 790
 *    84 -> Tiled GID 791
 *    85 -> Tiled GID 792
 *    86 -> Tiled GID 793
 *    87 -> Tiled GID 794
 *    88 -> Tiled GID 795
 *    89 -> Tiled GID 796
 *    90 -> Tiled GID 798
 *    91 -> Tiled GID 799
 *    92 -> Tiled GID 800
 *    93 -> Tiled GID 801
 *    94 -> Tiled GID 802
 *    95 -> Tiled GID 803
 *    96 -> Tiled GID 804
 *    97 -> Tiled GID 824
 *    98 -> Tiled GID 825
 *    99 -> Tiled GID 826
 *   100 -> Tiled GID 831
 *   101 -> Tiled GID 832
 *   102 -> Tiled GID 833
 *   103 -> Tiled GID 835
 *   104 -> Tiled GID 836
 *   105 -> Tiled GID 837
 *   106 -> Tiled GID 838
 *   107 -> Tiled GID 839
 *   108 -> Tiled GID 840
 *   109 -> Tiled GID 841
 *   110 -> Tiled GID 861
 *   111 -> Tiled GID 862
 *   112 -> Tiled GID 863
 *   113 -> Tiled GID 868
 *   114 -> Tiled GID 869
 *   115 -> Tiled GID 870
 *   116 -> Tiled GID 871
 *   117 -> Tiled GID 899
 *   118 -> Tiled GID 900
 *   119 -> Tiled GID 901
 *   120 -> Tiled GID 905
 *   121 -> Tiled GID 906
 *   122 -> Tiled GID 907
 *   123 -> Tiled GID 908
 *   124 -> Tiled GID 936
 *   125 -> Tiled GID 937
 *   126 -> Tiled GID 938
 *   127 -> Tiled GID 942
 *   128 -> Tiled GID 943
 *   129 -> Tiled GID 944
 *   130 -> Tiled GID 945
 *   131 -> Tiled GID 973
 *   132 -> Tiled GID 974
 *   133 -> Tiled GID 975
 *   134 -> Tiled GID 979
 *   135 -> Tiled GID 980
 *   136 -> Tiled GID 981
 *   137 -> Tiled GID 982
 */
static const uint8_t display_ground[] = {
      1u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   5u,
      6u,   8u,  14u,  15u,  15u,  15u,  15u,  15u,  16u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,  10u,   8u,  14u,  15u,  15u,  15u,  15u,  15u,  16u,   8u,  10u,
      6u,  12u,  17u,  18u,  18u,  18u,  18u,  18u,  19u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  10u,  12u,  17u,  18u,  18u,  18u,  18u,  18u,  19u,  12u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  10u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  62u,  10u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  72u,  10u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  89u,  10u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u, 102u,  10u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  10u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  10u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,  23u,   3u,   3u,   3u,   3u,   3u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   5u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,   8u,  10u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  12u,  10u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
      6u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,  40u,  41u,  39u,  40u,  41u,  39u,  40u,  41u,  10u,
      6u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,  43u,  44u,  42u,  43u,  44u,  42u,  43u,  44u,  10u,
     20u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   3u,   2u,   3u,   3u,   3u,   3u,   3u,   4u,   3u,  21u,
     35u,  35u,  25u,  32u,  33u,  33u,  33u,  33u,  34u,  25u,  35u,  35u,  25u,  32u,  33u,  33u,  33u,  33u,  34u,  25u,  35u,  35u,  25u,  26u,  35u,  35u,  35u,  35u,  35u,  36u,  25u,  35u,
     37u,  37u,  27u,  29u,  30u,  30u,  30u,  30u,  31u,  27u,  37u,  37u,  27u,  29u,  30u,  30u,  30u,  30u,  31u,  27u,  37u,  37u,  27u,  28u,  37u,  37u,  37u,  37u,  37u,  38u,  27u,  37u,
};
static const uint8_t display_objects[] = {
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  55u,  56u,  57u,   0u,  55u,  56u,  57u,   0u,  55u,  56u,  57u,  22u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  22u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  59u,  60u,  61u,   0u,  59u,  60u,  61u,   0u,  59u,  60u,  61u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  67u,  68u,  69u,   0u,  67u,  68u,  69u,   0u,  67u,  68u,  69u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 116u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  84u,  85u,  86u,   0u,  84u,  85u,  86u,   0u,  84u,  85u,  86u,   0u,   0u,   0u,   0u,   0u,  64u,  65u,  66u,   0u,   0u, 123u,   0u,
      0u, 116u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  81u,  82u,  83u,   0u,   0u, 130u,   0u,
      0u, 123u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  97u,  98u,  99u,   0u,  62u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 137u,   0u,
      0u, 130u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 127u, 128u, 129u,   0u,   0u,   0u, 110u, 111u, 112u,   0u,  72u,   0u,   0u,   0u,   0u,  64u,  65u,  66u,   0u,   0u,   0u,   0u,
      0u, 137u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 134u, 135u, 136u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  89u,   0u,   0u,   0u,   0u,  81u,  82u,  83u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 102u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
     24u,   4u,   0u,   0u,   0u,   0u,   0u,   2u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   4u,   0u,   0u,   0u,   0u,
      0u,   9u,   0u,   0u,   0u,   0u,   0u,   7u,   0u,   0u,  47u,  48u,   0u, 113u, 114u, 115u,   0u,  45u,  46u,   0u,  70u,  71u,  70u,  71u,  97u,  98u,  99u,   9u,   0u,   0u,   0u,   0u,
      0u,  13u,   0u,   0u,   0u,   0u,   0u,  11u,  49u,  50u,  51u,   0u,   0u, 120u, 121u, 122u,   0u,  64u,  65u,  66u,  87u,  88u,  87u,  88u, 110u, 111u, 112u,  13u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  52u,  53u,  54u,   0u,   0u,   0u,   0u,   0u,   0u,  81u,  82u,  83u, 100u, 101u, 100u, 101u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u, 116u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u, 123u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u, 130u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u, 137u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
     24u,   4u,   0u,   0u,   0u,   0u,   0u,   2u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,   9u,   0u,   0u,   0u,   0u,   0u,   7u, 117u, 118u, 119u,  55u,  56u,  57u,   0u,  55u,  56u,  57u,   0u,  55u,  56u,  22u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,
      0u,  13u,   0u,   0u,   0u,   0u,   0u,  11u, 124u, 125u, 126u,  59u,  60u,  61u,   0u,  59u,  60u,  61u,   0u,  59u,  60u,  61u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  62u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 131u, 132u, 133u,  67u,  68u,  69u,   0u,  67u,  68u,  69u,   0u,  67u,  68u,  69u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  72u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  84u,  85u,  86u,   0u,  84u,  85u,  86u,   0u,  84u,  85u,  86u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  89u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 116u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 102u,   0u,
      0u,   0u,   0u,  58u,   0u,   0u,  58u,   0u,   0u,  58u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 123u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  62u,   0u,
      0u,   0u,   0u,  63u,   0u,   0u,  63u,   0u,   0u,  63u,   0u,   0u,  64u,  65u,  66u,   0u,   0u,   0u,   0u,   0u,   0u, 130u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  72u,   0u,
      0u,   0u,   0u,  80u,   0u,   0u,  80u,   0u,   0u,  80u,   0u,   0u,  81u,  82u,  83u,   0u,   0u,   0u,   0u,   0u,   0u, 137u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  89u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 102u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  73u,  74u,  75u,  76u,  77u,  78u,  79u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,  90u,  91u,  92u,  93u,  94u,  95u,  96u,   0u,   0u,
      0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u, 103u, 104u, 105u, 106u, 107u, 108u, 109u,   0u,   0u,
};
static const uint8_t display_collision[] = {
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,
      1u,   1u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   0u,   0u,   0u,   0u,   0u,   0u,   0u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   0u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
      1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,   1u,
};
static const GeneratedEntity_t display_entities[] = {
    {GENERATED_ENTITY_BOSS_DIAL, 11u, 21u, LEVEL_COMPLETE}, /* dial1 */
    {GENERATED_ENTITY_BOSS_DIAL, 15u, 21u, LEVEL_COMPLETE}, /* dial2 */
    {GENERATED_ENTITY_BOSS_DIAL, 19u, 21u, LEVEL_COMPLETE}, /* dial3 */
    {GENERATED_ENTITY_BOSS_DIAL, 9u, 2u, LEVEL_COMPLETE}, /* dial4 */
    {GENERATED_ENTITY_BOSS_DIAL, 13u, 2u, LEVEL_COMPLETE}, /* dial5 */
    {GENERATED_ENTITY_BOSS_DIAL, 17u, 2u, LEVEL_COMPLETE}, /* dial6 */
    {GENERATED_ENTITY_BOSS_SPAWN, 10u, 15u, LEVEL_DISPLAY_BOSS}, /* bossspawnn */
    {GENERATED_ENTITY_MODULE_EXIT, 23u, 28u, LEVEL_COMPLETE}, /* doorback2 */
    {GENERATED_ENTITY_PLAYER_SPAWN, 25u, 25u, LEVEL_COMPLETE}, /* landing */
};

static const GeneratedMapData_t generated_maps[] = {
    {32u, 21u, overworld_ground, overworld_objects, overworld_collision, overworld_entities, 17u},
    {28u, 28u, speaker_ground, speaker_objects, speaker_collision, speaker_entities, 12u},
    {32u, 32u, display_ground, display_objects, display_collision, display_entities, 9u},
};

const GeneratedMapData_t* generated_map_get(GeneratedMapId_t id)
{
    if (id > GENERATED_MAP_DISPLAY) {
        id = GENERATED_MAP_OVERWORLD;
    }

    return &generated_maps[id];
}

uint8_t generated_map_tile_at(const GeneratedMapData_t* map, const uint8_t* layer, uint8_t tile_x, uint8_t tile_y)
{
    if (map == 0 || layer == 0 || tile_x >= map->width || tile_y >= map->height) {
        return 0u;
    }

    return layer[((uint16_t)tile_y * map->width) + tile_x];
}

uint8_t generated_map_find_entity(const GeneratedMapData_t* map, uint8_t entity_type, uint8_t occurrence, GeneratedEntity_t* out_entity)
{
    uint8_t seen = 0u;

    if (map == 0 || out_entity == 0) {
        return 0u;
    }

    for (uint8_t i = 0u; i < map->entity_count; i++) {
        if (map->entities[i].type != entity_type) {
            continue;
        }

        if (seen == occurrence) {
            *out_entity = map->entities[i];
            return 1u;
        }

        seen++;
    }

    return 0u;
}
