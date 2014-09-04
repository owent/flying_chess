using System;
using System.Collections.Generic;

namespace OverCraft.Data {
	public class Plane {

		public object Clone() {
			var p = new Plane();
			p._color = _color;
			p._containGrid = _containGrid;
			p._isCompleted = _isCompleted;
			p._isStarted = _isStarted;
			p._prevGrid = _prevGrid;
			p._startGrid = _startGrid;
			p._times = _times;
			return p;
		}

		public Plane(int color = 0, Grid Containner = null) {
			_isCompleted = false;
			_isStarted = false;
			_color = color;
			_startGrid = _containGrid = Containner;
			_times = 0;
		}

		public int Color {
			get {
				return _color;
			}
		}
		protected int _color;

		protected Grid _startGrid;

		public Grid ContainGrid {
			set {
				_containGrid = value;
			}
			get {
				return _containGrid;
			}
		}
		public Grid PrevGrid {
			get {
				return _prevGrid;
			}
		}
		protected Grid _containGrid;
		protected Grid _prevGrid;

		public bool IsCompleted {
			get {
				return _isCompleted;
			}
		}
		protected bool _isCompleted;

		public bool IsStarted {
			get {
				return _isStarted;
			}
		}
		protected bool _isStarted;

		public int Times {
			get {
				return _times;
			}
		}
		protected int _times;

		protected void moveNext() {
			var _tmpGrid = _containGrid;
			_containGrid = _containGrid.NextGrid(this);
			_prevGrid = _tmpGrid;
		}

		protected void moveWhenNotStarted(int times) {
			if (_times == 6) {
				_isStarted = true;
				moveNext();
			}
			return;
		}

		protected void moveWhenStarted(int times) {
			while ((_times--) > 0)
				moveNext();
		}

		protected void deleteInPrevGrid() {
			_containGrid.DeletePlane(this);
		}

		public void Move(int times) {
			_prevGrid = null;
			_times = times;
			deleteInPrevGrid();
			if (!IsStarted) {
				moveWhenNotStarted(times);
				return;
			}
			moveWhenStarted(times);
		}

		protected void moveJump() {
			_prevGrid = _containGrid;
			_containGrid = _containGrid.JumpGrid;
		}

		protected void flyingCrossWhenJump() {
			var t_grid = (StraightGrid)((FlyingGrid)ContainGrid).CrossGrid;
			t_grid.Check(this);
		}

		protected void doJumpping() {
			moveJump();
			ContainGrid.Check(this, true);
		}

		public void Jump(bool isFlying = false) {
			deleteInPrevGrid();
			if (isFlying) {
				flyingCrossWhenJump();
			}
			doJumpping();
		}

		public void removePrev() {
			_prevGrid = null;
		}

		public void GoHome() {
			_prevGrid = _containGrid;
			_containGrid = _startGrid;
			_isStarted = false;
		}

		public void Complete() {
			_isCompleted = true;
			_isStarted = false;
			_containGrid = _startGrid;
		}
	}
}
