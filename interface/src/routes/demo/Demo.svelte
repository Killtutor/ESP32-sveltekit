<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import Light from '~icons/tabler/bulb';
	import MdiWaterPump from '~icons/mdi/water-pump';
	import MdiWaterPumpOff from '~icons/mdi/water-pump-off';
	import { socket } from '$lib/stores/socket';
	import type { WaterSupply } from '$lib/types/models';

	let waterState: WaterSupply = { waterIn: false, waterLevel: 10 };

	async function getWaterstate() {
		try {
			console.log($page.data.features.security)
			const response = await fetch('/rest/waterState', {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic 123',
					'Content-Type': 'application/json',
					"Cache-Control": "no-cache"
				}
			});
			console.log("🚀 ~ getWaterstate ~ response:", response)
			const data = await response.json();
			console.log("🚀 ~ getWaterstate ~ data:", data)
			waterState = data;
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	onMount(() => {
		socket.on<WaterSupply>('water', (data) => {
		console.log("🚀 ~ onMount ~ data:", data)
			waterState = data;
		});
		getWaterstate();
	});

	onDestroy(() => socket.off('water'));
</script>

<SettingsCard collapsible={false}>
	<Light slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Estado del Agua Edificio Danny</span>
	<div class="w-full">
		<h1 class="text-xl font-semibold">Entrada de Agua</h1>
		<div class="alert alert-info my-2 shadow-lg bg-orange-300">
			<MdiWaterPump class="h-6 w-6 flex-shrink-0 stroke-current" />
			<span class="text-green-600">Indica entrada de agua.</span>

			<MdiWaterPumpOff class="h-6 w-6 flex-shrink-0 stroke-current" />
			<span class="text-red-600">Indica ausencia de agua.</span>
		</div>
		<div class="flex flex-row flex-wrap justify-between gap-x-2">

			{#if waterState.waterIn}

			<div class=" w-full text-green-600 font-semibold h-4">
					<MdiWaterPump class="h-6 w-6 flex-shrink-0 stroke-current" />
					<span class="mr-4">Esta entrando agua, quedate tranquilo</span>
			</div>
			{:else}

			<div class=" w-full text-red-600 font-semibold h-4">
					<MdiWaterPumpOff class="h-6 w-6 flex-shrink-0 stroke-current" />
					<span class="mr-4">Pendiente con el nivel del Agua</span>
			</div>
			{/if}
		</div>
		<div class="divider" />
		<h1 class="text-xl font-semibold">Aqui se vendra lo del nivel del agua</h1>
	</div>
</SettingsCard>
