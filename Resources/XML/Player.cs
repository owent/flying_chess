using System.Collections.Generic;
using System.Linq;

namespace OverCraft.Data {
	public class Player {

		protected List<Plane> _planeList;

		public List<Plane> PlaneList { get { return _planeList; } }

		public Player(List<Plane> planeList = null, int color = 0) {
			_planeList = new List<Plane>();
			if (planeList != null) {
				_planeList = planeList;
			}
			_color = color;
		}

		public int Color {
			get {
				return _color;
			}
		}
		protected int _color;

		public int PlaneNumber {
			get {
				return _planeList.Count;
			}
		}

		public Plane this[int index] {
			get {
				return _planeList[index];
			}
		}

		public bool IsComplete {
			get {
				return _planeList.All(plane => plane.IsCompleted);
			}
		}
	}
}
