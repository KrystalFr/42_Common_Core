module.exports = {
  preset: 'ts-jest/presets/default-esm',
  testEnvironment: 'node',

  extensionsToTreatAsEsm: ['.ts'],

  resolver: 'ts-jest-resolver',

  transform: {
    '^.+\\.tsx?$': [
      'ts-jest',
      {
        useESM: true,
        tsconfig: '<rootDir>/tsconfig.test.json',
      },
    ],
  },

  roots: ['<rootDir>/test'],
  moduleFileExtensions: ['ts', 'js', 'mjs', 'cjs', 'json'],
  collectCoverageFrom: ['src/**/*.ts'],
  coverageDirectory: 'coverage',
  coverageReporters: ['text', 'lcov', 'html'],
};
