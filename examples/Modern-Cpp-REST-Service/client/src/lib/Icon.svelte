<script>
  export let name;
  export let size = 20;
</script>

<svg
  viewBox="0 0 24 24"
  width={size}
  height={size}
  fill="none"
  stroke="currentColor"
  stroke-width="1.8"
  stroke-linecap="round"
  stroke-linejoin="round"
  aria-hidden="true"
>
  {#if name === 'overview'}
    <rect x="3" y="3" width="7" height="7" rx="2" />
    <rect x="14" y="3" width="7" height="7" rx="2" />
    <rect x="3" y="14" width="7" height="7" rx="2" />
    <rect x="14" y="14" width="7" height="7" rx="2" />
  {:else if name === 'device'}
    <rect x="5" y="3" width="14" height="18" rx="3" />
    <path d="M9 7h6M9 11h6M9 15h3" />
    <circle cx="16" cy="16" r="1" />
  {:else if name === 'control'}
    <path d="M4 7h10M18 7h2M4 17h2M10 17h10" />
    <circle cx="16" cy="7" r="2" />
    <circle cx="8" cy="17" r="2" />
  {:else if name === 'pulse'}
    <path d="M3 12h4l2.5-6 5 12 2.5-6H21" />
  {:else if name === 'terminal'}
    <rect x="3" y="4" width="18" height="16" rx="3" />
    <path d="m7 9 3 3-3 3M13 15h4" />
  {:else if name === 'refresh'}
    <path d="M20 7v5h-5M4 17v-5h5" />
    <path d="M6.1 8.5A7 7 0 0 1 18.7 7M5.3 17A7 7 0 0 0 17.9 15.5" />
  {:else if name === 'bolt'}
    <path d="m13 2-8 12h7l-1 8 8-12h-7z" />
  {:else if name === 'shield'}
    <path d="M12 3 5 6v5c0 4.5 2.8 8 7 10 4.2-2 7-5.5 7-10V6z" />
    <path d="m9 12 2 2 4-4" />
  {:else if name === 'chevron'}
    <path d="m9 18 6-6-6-6" />
  {:else if name === 'activity'}
    <path d="M4 18V9M10 18V5M16 18v-7M22 18V3" />
  {:else if name === 'menu'}
    <path d="M4 7h16M4 12h16M4 17h16" />
  {:else if name === 'close'}
    <path d="m6 6 12 12M18 6 6 18" />
  {/if}
</svg>

