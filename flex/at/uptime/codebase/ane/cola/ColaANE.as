package at.uptime.codebase.ane.cola {
	import flash.events.EventDispatcher;
	import flash.external.ExtensionContext;

	public class ColaANE extends EventDispatcher {
		public static const ID : String = "at.uptime.codebase.ane.cola.ColaANE";

		private var extContext : ExtensionContext;

		public function ColaANE() {
			extContext = ExtensionContext.createExtensionContext(ID, null);
			trace("Cola extension loaded? ", (extContext != null ? "yes" : "no"));
		}

		public function start(
			vertices: Array,
			edges: Array,
			defaultLength: Number,
			iterationsWithoutOverlapChecks: int = 10,
			threshold: Number = 0.0001,
			maxIterations: int = 100,
			sizeStepMultiplier: Number = 1.0,
			adjustEdges: Boolean = false,
			adjustEdgeLengthWeight: Number = 0.5
		): Boolean {
			var response: Object = extContext.call(
				"ColaStartLayout",
				vertices,
				edges,
				defaultLength,
				iterationsWithoutOverlapChecks,
				threshold,
				maxIterations,
				sizeStepMultiplier,
				adjustEdges,
				adjustEdgeLengthWeight
			);

			if (response.hasOwnProperty('error') && response.error != null) {
				throw new Error(response.error);
			}

			return response.result;
		}

		public function tick(removeOverlapsAfterwards: Boolean = true): Boolean {
			var response: Object = extContext.call("ColaTick", removeOverlapsAfterwards);

			if (response.hasOwnProperty('error') && response.error != null) {
				throw new Error(response.error);
			}

			return response.result;
		}

		public function run(): Array {
			var response: Object = extContext.call("ColaRun");

			if (response.hasOwnProperty('error') && response.error != null) {
				throw new Error(response.error);
			}

			return response.result;
		}

		public function getVertices(): Array {
			var response: Object = extContext.call("ColaGetVertices");

			if (response.hasOwnProperty('error') && response.error != null) {
				throw new Error(response.error);
			}

			return response.result;
		}

		public function removeOverlaps(): Boolean {
			var response: Object = extContext.call("ColaRemoveOverlaps");

			if (response.hasOwnProperty('error') && response.error != null) {
				throw new Error(response.error);
			}

			return response.result;
		}

		public function dispose() : void
		{
			trace("Cola extension dispose");			
			extContext.dispose();
		}
	}
}
