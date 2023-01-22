import { writeFileSync } from 'fs';

// FIXME fine a way to get ZIC_WAVETABLES_MAX_SAMPLES from c++...
export const WAVETABLE_SIZE = 2048;

export function save(table: number[], name: string, float = true) {
    const capName = name[0].toUpperCase() + name.slice(1);

    const data = float ? `
    float table[ZIC_WAVETABLES_MAX_SAMPLES] = {${table
        .map((val) => (val === 0 || val === 1 || val === -1 ? `${val}.0f` : `${val}f`))
        .join(', ')},};
    `: `
    int16_t table[ZIC_WAVETABLES_MAX_SAMPLES] = {${table
        .map((val) => Math.round(val * 10000))
        .join(', ')},};
    `;

    const content = `
#ifndef WAVETABLE_${name.toUpperCase()}_H_
#define WAVETABLE_${name.toUpperCase()}_H_

#ifndef ZIC_WAVETABLES_MAX_SAMPLES
#define ZIC_WAVETABLES_MAX_SAMPLES 2048
#endif

class Wavetable_${capName}
{
public:
${data}
};

#endif
`;
    writeFileSync(`../wavetable_${name}.h`, content);
}
