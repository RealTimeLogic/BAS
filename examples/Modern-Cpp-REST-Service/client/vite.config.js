import { defineConfig } from 'vite';
import { svelte } from '@sveltejs/vite-plugin-svelte';

const apiOrigin = process.env.BAS_REST_API_ORIGIN || 'http://127.0.0.1:9357';

export default defineConfig({
  plugins: [svelte()],
  server: {
    proxy: {
      '/api': {
        target: apiOrigin,
        changeOrigin: true
      }
    }
  },
  build: {
    target: 'es2020',
    assetsInlineLimit: 4096
  }
});

