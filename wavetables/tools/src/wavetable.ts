import { writeFileSync } from 'fs';

export const WAVETABLE_SIZE = 2048;

export function save(table: number[], name: string, float = false) {
    const capName = name[0].toUpperCase() + name.slice(1);

    const data = float ? `
    float _table[${WAVETABLE_SIZE}] = {${table
        .map((val) => (val === 0 || val === 1 || val === -1 ? `${val}.0f` : `${val}f`))
        .join(', ')},};
    `: `
    int16_t _table[${WAVETABLE_SIZE}] = {${table
        .map((val) => Math.round(val * 10000))
        .join(', ')},};
    `;

    const content = `
#ifndef WAVETABLE_${name.toUpperCase()}_H_
#define WAVETABLE_${name.toUpperCase()}_H_

#include "../zic_wavetable_base.h"

class Wavetable_${capName} : public Zic_Wavetable_Base
{
public:
    Wavetable_${capName}() : Zic_Wavetable_Base(&_table[0], ${WAVETABLE_SIZE}, ${WAVETABLE_SIZE})
    {
    }
protected:
${data}
};

Wavetable_${capName} wavetable_${capName};

#endif
`;
    writeFileSync(`../wavetable_${name}.h`, content);
}
